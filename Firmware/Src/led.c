#include "led.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "color.h"
#include "iprintf.h"

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

#define MATRIX_ROWS              (4)
#define MATRIX_COLS              (12)
#define ROW_BLANKING             (4)

//TODO adjust to adjust brightness/update frq
//the number of timer ticks to wait before turning this row off
#define COUNTS_FOR_PERSISTANCE   (1)

//FIXME move
enum DrawState {
   //FIXME needed?
   DS_Start,

   //TODO blanking state needed?
   DS_BlankRow,
   DS_WriteNewRow,
   DS_EnableRow,
   DS_PauseForEffect,
   DS_DisableRow,
};

struct MatrixState {
   // the last row that was displayed
   uint8_t row;

   // number of counts waited for persistance
   uint16_t waitCounts;

   // state machine state
   enum DrawState stage;
};
static struct MatrixState matrixState = {.row = 0, .stage = DS_Start};

// the in-memory version of the entire matrix
//remember this is in 3 byte RGB groups (so it's 12 * 3 wide)
//5th row is black
static struct color_ColorRGB matrix[MATRIX_ROWS + 1][MATRIX_COLS] = {0};

// static LUT for controlling matrix row FETs
static uint16_t const MatrixPinLUT[] =     {GPIO_PIN_8, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
static GPIO_TypeDef * const MatrixPortLUT[] = {GPIOA, GPIOB, GPIOB, GPIOB};

static void _ConfigureFrameClock(void);
static bool _EnableChannel(uint8_t chan, enum led_Divisor div);
static bool _ForceUpdateRow(void);
static bool _WriteRow(int rowIndex);

void led_Init(void){
   HAL_StatusTypeDef stat;
   uint8_t data[63 + 10] = {};

   // disable SW shutdown
   data[0] = REG_SHUTDOWN;
   data[1] = 0x1;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, data, 2, 1000);
   if(stat != 0) {
      iprintf("Stat = 0x%x\n", stat);
   }

   // set enable bit and scalar on all channels
   for(int i = 0; i < 36; i++) {
      _EnableChannel(i, DIVISOR_4);
   }

   _ForceUpdateRow();

   // enable all channels
   data[0] = REG_GLOBAL_CONTROL;
   data[1] = 0x0;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, data, 2, 1000);

   // start up the frame clock. After this, it will be drawing to the display
   _ConfigureFrameClock();
}

void led_ClearDisplay(void) {
   //TODO actually disable the channels to save power

   memset(matrix, '\0', sizeof(matrix));
   _ForceUpdateRow();
}

// Update the in-memory matrix representation
void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV color) {
   if(x > MATRIX_COLS || y > MATRIX_ROWS) {
      iprintf("Illegal row/col request (x,y) (%d,%d)\n", x, y);
      return;
   }

   color_HSV2RGB(&color, &matrix[y][x]);
}

// call to complete an entire draw cycle immediately
void led_ForceRefresh(void) {
   int i;
   for(i = 0; i < MATRIX_ROWS; i++) {
      //write black
      //blanking is only needed if we aren't always displaying colors. If we are then
      //ghostingisn't really visible
      //_WriteRow(ROW_BLANKING);

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
         iprintf("matrix SM start\n");

         // any init needed
         matrixState.waitCounts = 0;

         // progress SM
         //matrixState.stage = DS_BlankRow;
         matrixState.stage = DS_WriteNewRow;
         break;

      case DS_BlankRow:
         // start i2c to write the blank row
         _WriteRow(ROW_BLANKING);

         // progress SM
         matrixState.stage = DS_WriteNewRow;
         break;

      case DS_WriteNewRow:
         // write new data to controller for this col while everything is off in ONE ARRAY SEND
         _WriteRow(matrixState.row);

         // progress SM
         matrixState.stage = DS_EnableRow;
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
         iprintf("Matrix SM default state. Why?\n");
         break;
   }
}

static bool _WriteRow(int rowIndex) {
   HAL_StatusTypeDef stat;
   //TODO macro base size
   uint8_t config[36 + 1 + 1] = {0};

   //set the register
   config[0] = REG_PWM_BASE;
   //set the final byte to force the controller to update its outputs
   config[36 + 1] = 0x0;

   //FIXME better way? somehow expose a buffer to write into??
   //FIXME macro over memcpy size
   memcpy(&config[1], matrix[rowIndex], 36);

   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 100);
   if(stat != 0) {
      iprintf("Stat = 0x%x\n", stat);
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
      iprintf("Stat = 0x%x\n", stat);
      return false;
   }
   return true;
}

//update the contents of the display with what's in its memory
static bool _ForceUpdateRow(void) {
   HAL_StatusTypeDef stat;
   uint8_t config[2] = {};

   config[0] = REG_PWM_UPDATE;
   config[1] = 0x0;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 1000);
   if(stat != 0) {
      iprintf("Stat = 0x%x\n", stat);
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
   HAL_TIM_Base_Init(&htim14);

   led_MatrixStart();
}

void led_MatrixStart(void) {
   HAL_TIM_Base_Start_IT(&htim14);
}

void led_MatrixStop(void) {
   HAL_TIM_Base_Stop_IT(&htim14);
}

