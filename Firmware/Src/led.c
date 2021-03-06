#include "led.h"
#include "led_test.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "color.h"
#include "iprintf.h"
#include "platform_hw.h"
#include "matrix_assignments.h"

#include <stdint.h>
#include <string.h>

//FIXME move elsewhere
extern I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim14;

// can only write (0b0)
#define LED_CONT_ADDR            (0x78)

#define REG_SHUTDOWN             (0x00)
#define REG_PWM_BASE             (0x01)
#define REG_PWM_UPDATE           (0x25)
#define REG_LED_CONTROL_BASE     (0x26)
#define REG_GLOBAL_CONTROL       (0x4A)
#define REG_RESET                (0x4F)

enum led_Divisor {
   DIVISOR_NONE   = 0x0,
   DIVISOR_2      = 0x1,
   DIVISOR_3      = 0x2,
   DIVISOR_4      = 0x3,
};

#define ROW_BLANKING             (4)

//the number of timer ticks to wait before turning this row off
#define COUNTS_FOR_PERSISTANCE   (1)

enum DrawState {
   DS_Start,

   DS_BlankRow,         //optional, enable if needed to reduce ghosting
   DS_WriteNewRow,
   DS_EnableRow,
   DS_PauseForEffect,
   DS_DisableRow,
};

enum WriteState {
   ws_Idle,
   ws_Progress,
   ws_Done,
};
// tracks the state of the i2c write
static enum WriteState writeState = ws_Idle;
static uint8_t rowWriteBuf[LED_CHANNELS + 1 + 1] = {0};

struct MatrixState {
   //TODO move?
   uint8_t brightness;

   // the last row that was displayed
   uint8_t row;

   // number of counts waited for persistance
   uint16_t waitCounts;

   // state machine state
   enum DrawState stage;
};
static struct MatrixState matrixState = {.row = 0, .stage = DS_Start};

struct ColorPointer {
   uint8_t *r;
   uint8_t *g;
   uint8_t *b;
};

// the underlying in-memory version of the entire matrix, including a fifth row of solid black
// for blanking.
// 4 arrays of 36 bytes long. Physical LEDs divide into groups of 3
//                           x              y
static uint8_t matrixRaw[LED_BANKS + 1][LED_CHANNELS] = {0};
// this is the mapping layer used to access the matrix logically
static struct ColorPointer matrixMapped[MATRIX_ROWS + 1][MATRIX_COLS];
// mapping layer for accessing the array linearly
static struct ColorPointer matrixLinear[TOTAL_LOGICAL_LEDS];

// static LUT for controlling matrix row FETs
static uint16_t const MatrixPinLUT[]         = {GPIO_PIN_8, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
static GPIO_TypeDef * const MatrixPortLUT[]  = {GPIOA, GPIOB, GPIOB, GPIOB};

static bool _ConfigureLEDController(void);
static void _ConfigureFrameClock(void);
static bool _EnableChannel(uint8_t chan, enum led_Divisor div);
static bool _ForceUpdateRow(void);
static bool _WriteRow(int rowIndex);
static void _configureMapping(void);

static void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV * color);
static void led_DrawSparse(uint8_t x, uint8_t y, struct color_ColorHSV * const color);
static void led_DrawRing(uint8_t r, struct color_ColorHSV * const color);

void led_Init(void){
   // most HW init in platform_hw and hal_msp

   // spin until LED init succeeds
   if(!_ConfigureLEDController()) {
      // this is aggressive, but if we fail to init the LED controller the easiest
      // fix seems to be to reset everthing (which is what the user would do anyway);
      HAL_Delay(500);
      HAL_NVIC_SystemReset();
   }

   // start up the frame clock. After this, it will be drawing to the display
   _ConfigureFrameClock();

   iprintf("LED Init Done\n");
}

void led_TestInit(void) {
   _ConfigureLEDController();

   // don't turn on the frame clock
}

bool _ConfigureLEDController(void) {
   HAL_StatusTypeDef stat;
   uint8_t data[63 + 10] = {};

   iprintf("Attempting an LED init...\n");

   matrixState.brightness = 255;

   // clear display
   memset(matrixRaw, 0, sizeof(matrixRaw));

   _configureMapping();

   // disable SW shutdown
   data[0] = REG_SHUTDOWN;
   data[1] = 0x1;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, data, 2, 1000);
   if(stat != 0) {
      iprintf("Disable shutdown failed. Stat = 0x%x\n", stat);
      return false;
   }

   // set enable bit and scalar on all channels
   for(int i = 0; i < LED_CHANNELS; i++) {
      if(!_EnableChannel(i, DIVISOR_4)) {
         return false;
      }
   }

   _ForceUpdateRow();

   // enable all channels
   data[0] = REG_GLOBAL_CONTROL;
   data[1] = 0x0;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, data, 2, 1000);
   if(stat != 0) {
      iprintf("Enable all channels failed. Stat = 0x%x\n", stat);
      return false;
   }

   led_ClearDisplay();

   return true;
}

void led_ClearDisplay(void) {
   //TODO actually disable the channels to save power

   memset(matrixRaw, '\0', sizeof(matrixRaw));
   _ForceUpdateRow();
}

void led_SetGlobalBrightness(enum led_Brightness bright, uint8_t sub) {
   enum led_Divisor internalDiv;
   switch(bright) {
      default:
      case LED_DIV_1:
         internalDiv = DIVISOR_NONE;
         break;
      case LED_DIV_2:
         internalDiv = DIVISOR_2;
         break;
      case LED_DIV_3:
         internalDiv = DIVISOR_3;
         break;
      case LED_DIV_4:
         internalDiv = DIVISOR_4;
         break;
   }

   led_Pause();

   // set enable bit and scalar on all channels
   for(int i = 0; i < LED_CHANNELS; i++) {
      _EnableChannel(i, internalDiv);
   }
   //led_UpdateDisplay();

   led_Resume();

   // set the max scalar (each channel can be set up to this)
   matrixState.brightness = sub;
}

void led_Pause(void) {
   led_MatrixStop();
}

void led_Resume(void) {
   led_MatrixStart();
}

// Update the in-memory matrix representation
static void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV * color) {
   if(x >= MATRIX_ROWS || y >= MATRIX_COLS) {
      iprintf("Illegal row/col request (x,y) (%d,%d)\n", x, y);
      return;
   }

   //disregard the V that was passed in and use global brightness
   color->v = matrixState.brightness;

   //FIXME do this more elegantly?
   struct color_ColorRGB rgb;
   color_HSV2RGB(color, &rgb);
   color_CIECorrect(&rgb);

   *matrixMapped[x][y].r = rgb.r;
   *matrixMapped[x][y].g = rgb.g;
   *matrixMapped[x][y].b = rgb.b;
}

void led_DrawPixelLinear(uint8_t x, struct color_ColorHSV * const color) {
   if(x >= TOTAL_LOGICAL_LEDS) {
      iprintf("Illegal linear request for LED %d\n", x);
      return;
   }

   //disregard the V that was passed in and use global brightness
   color->v = matrixState.brightness;

   //FIXME do this more elegantly?
   struct color_ColorRGB rgb;
   color_HSV2RGB(color, &rgb);
   color_CIECorrect(&rgb);

   *matrixLinear[x].r = rgb.r;
   *matrixLinear[x].g = rgb.g;
   *matrixLinear[x].b = rgb.b;
}

static void led_DrawSparse(uint8_t x, uint8_t y, struct color_ColorHSV * const color) {
   if(x >= MATRIX_SPARSE_WIDTH || y >= MATRIX_SPARSE_HEIGHT ) {
      iprintf("Illegal row/col request (x,y) (%d,%d)\n", x, y);
      return;
   }

   if(MatrixMapSparse[y][x] != MATRIX_NO_LED) {
      led_DrawPixelLinear(MatrixMapSparse[y][x], color);
   }
}

static void led_DrawRing(uint8_t r, struct color_ColorHSV * const color) {
   if(r >= MATRIX_POLAR_RINGS) {
      iprintf("Illegal ring request (%d)\n", r);
      return;
   }

   for(int i = 0; MatrixMapPolar[r][i] != MATRIX_NO_LED; i++) {
      led_DrawPixelLinear(MatrixMapPolar[r][i], color);
   }
}

// call to complete an entire draw cycle immediately
void led_ForceRefresh(void) {
   int i;
   for(i = 0; i < MATRIX_ROWS; i++) {
      //blanking is only needed if we aren't always displaying colors. If we are then
      // ghosting isn't very visible and this just slows things down
      _WriteRow(ROW_BLANKING);

      //write new data to controller for this col while everything is off in ONE ARRAY SEND
      _WriteRow(i);

      //enable the col to show
      HAL_GPIO_WritePin(MatrixPortLUT[i], MatrixPinLUT[i], GPIO_PIN_RESET);

      //persis so the human can see it?
      HAL_Delay(1);

      // disable this row
      HAL_GPIO_WritePin(MatrixPortLUT[i], MatrixPinLUT[i], GPIO_PIN_SET);
   }
}

/*
 * This is the main drawing function. The expectation is that someone calls it VERY
 * FREQUENTLY, otherwise nothing will be displayed! If it's called slowly there may
 * be flicker. Calling it from a 1kHz timer ISR seems to work well (yes, it's all run
 * in ISR context, even the i2c).
 */
void led_UpdateDisplay(void) {
   switch(matrixState.stage) {
      case DS_Start:
         // any init needed
         writeState = ws_Idle;
         matrixState.row = 0;
         matrixState.waitCounts = 0;

         // progress SM
         matrixState.stage = DS_WriteNewRow;
         break;

      case DS_BlankRow:
         // start i2c to write the blank row
         if(writeState == ws_Idle) {
            _WriteRow(ROW_BLANKING);
         }

         // progress SM if i2c is done sending
         if(writeState == ws_Done) {
            writeState = ws_Idle;
            matrixState.stage = DS_DisableRow;
         }
         break;

      case DS_WriteNewRow:
         // write new data to controller for this col while everything is off in ONE ARRAY SEND
         if(writeState == ws_Idle) {
            _WriteRow(matrixState.row);
         }

         // progress SM
         if(writeState == ws_Done) {
            writeState = ws_Idle;
            matrixState.stage = DS_EnableRow;
         }
         break;

      case DS_EnableRow:
         // enable current row
         HAL_GPIO_WritePin(MatrixPortLUT[matrixState.row], MatrixPinLUT[matrixState.row], GPIO_PIN_RESET);

         // progress SM
         matrixState.stage = DS_PauseForEffect;
         break;

      case DS_PauseForEffect:
         matrixState.waitCounts++;
         if(matrixState.waitCounts > COUNTS_FOR_PERSISTANCE) {
            matrixState.waitCounts = 0;

            //blank for one cycle
            //matrixState.stage = DS_BlankRow;

            //progress SM
            matrixState.stage = DS_DisableRow;
         }

         break;

      case DS_DisableRow:
         // disable this row
         HAL_GPIO_WritePin(MatrixPortLUT[matrixState.row], MatrixPinLUT[matrixState.row], GPIO_PIN_SET);

         //progress to next row
         matrixState.row++;
         if(matrixState.row >= MATRIX_ROWS) {
            matrixState.row = 0;
         }

         // progress SM
         matrixState.stage = DS_WriteNewRow;
         break;

      default:
         //should never happen
         iprintf("Matrix SM hit default state. Why?\n");
         break;
   }
}

// called by the HAL whenever an i2c transfer ends
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
   writeState = ws_Done;
}

// enable one bank and disable the others
void led_TestExEnableBank(enum led_TestBankID bank) {
   for(int i = LED_TBANK_START; i < LED_TBANK_END; i++) {
      if(i == bank) {
         // enable
         HAL_GPIO_WritePin(MatrixPortLUT[i], MatrixPinLUT[i], GPIO_PIN_RESET);
      }
      else {
         // disable
         HAL_GPIO_WritePin(MatrixPortLUT[i], MatrixPinLUT[i], GPIO_PIN_SET);
      }
   }
}

void led_TestDrawPixel(uint8_t x, uint8_t y, struct color_ColorRGB * color) {
   // base 0, so if there are 3 rows valid indicies are 0, 1, 2. Asking for 3 is too many
   if(x >= MATRIX_ROWS || y >= MATRIX_COLS) {
      iprintf("Illegal row/col request (x,y) (%d,%d)\n", x, y);
      return;
   }

   *matrixMapped[x][y].r = color->r;
   *matrixMapped[x][y].g = color->g;
   *matrixMapped[x][y].b = color->b;
}

static bool _WriteRow(int rowIndex) {
   HAL_StatusTypeDef stat;

   //set the register
   rowWriteBuf[0] = REG_PWM_BASE;
   //set the final byte to force the controller to update its outputs
   rowWriteBuf[LED_CHANNELS + 1] = 0x0;

   //TODObetter way? somehow expose a buffer to write into?
   //TODO DMA this
   memcpy(&rowWriteBuf[1], &matrixRaw[rowIndex][0], LED_CHANNELS);

   // FIXME make IT?
   writeState = ws_Progress;
   stat = HAL_I2C_Master_Transmit_IT(&hi2c1, LED_CONT_ADDR, rowWriteBuf, sizeof(rowWriteBuf));
   if(stat != 0) {
      iprintf("Row Stat = 0x%x\n", stat);
      return false;
   }
   return true;
}

static bool _EnableChannel(uint8_t chan, enum led_Divisor div) {
   HAL_StatusTypeDef stat;
   uint8_t config[2] = {};

   config[0] = REG_LED_CONTROL_BASE + chan;
   // enable this channel, and set the current divisor
   config[1] = (0x1 << 0) | (div << 1);

   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 1000);
   if(stat != 0) {
      iprintf("En Chan Stat = 0x%x\n", stat);
      return false;
   }
   return true;
}

void led_TestRefresh(enum led_TestBankID bank) {
   // write a row
   _WriteRow(bank);

   _ForceUpdateRow();
}

//update the contents of the display with what's in its memory
static bool _ForceUpdateRow(void) {
   HAL_StatusTypeDef stat;
   uint8_t config[2] = {};

   config[0] = REG_PWM_UPDATE;
   config[1] = 0x0;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 1000);
   if(stat != 0) {
      iprintf("Update Channels Stat = 0x%x\n", stat);
      return false;
   }
   return true;
}

static void _ConfigureFrameClock(void) {
   htim14.Instance = TIM14;
   htim14.Init.Prescaler = 100;
   htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim14.Init.Period = 241;
   htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   htim14.Init.RepetitionCounter = 0;
   //make sure we disable all output channels (we just want interrupts)
   htim14.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
   HAL_TIM_Base_Init(&htim14);

   led_MatrixStart();
}

// Setup all the mapping used to address the matrix
static void _configureMapping(void) {
   iprintf("Setting up matrix interposer...\n");
   for(int i = 0; i < TOTAL_CHANNELS; i++) {
      struct matrixMap const * const m = &MatrixMap[i];
      uint8_t * const r = &matrixRaw[m->bank][m->ch];
      // set the pointers in matrixMapped to point to the correct elements in
      // the matrixRaw below it

      //FIXME rm
      //iprintf("%d,%d -> b %d,ch %d\n", m->row, m->col, m->bank, m->ch);

      // add an offset of 12 for each bank (0-3) to calculate the linear position
      int const linearOffset = m->linear + LinearMatrixBankOffsets[m->row];

      switch(m->color) {
         case MMC_RED:
            matrixMapped[m->row][m->col].r = r;
            matrixLinear[linearOffset].r = r;
            break;
         case MMC_GREEN:
            matrixMapped[m->row][m->col].g = r;
            matrixLinear[linearOffset].g = r;
            break;
         case MMC_BLUE:
            matrixMapped[m->row][m->col].b = r;
            matrixLinear[linearOffset].b = r;
            break;
      }
   }

   iprintf("Done\n");
}

void led_MatrixStart(void) {
   HAL_TIM_Base_Start_IT(&htim14);
}

void led_MatrixStop(void) {
   HAL_TIM_Base_Stop_IT(&htim14);
}

