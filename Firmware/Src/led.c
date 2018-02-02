#include "led.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "color.h"
#include "iprintf.h"

#include <stdint.h>
#include <string.h>

//FIXME move elsewhere
extern I2C_HandleTypeDef hi2c1;
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

static bool _EnableChannel(uint8_t chan, enum led_Divisor div);
static bool _ForceUpdate(void);
static bool _SetChannelRaw(uint8_t chan, uint8_t intensity);
static bool _WriteRow(int rowIndex);

// the in-memory version of the entire matrix
//TODO macro size
//remember this is in 3 byte colors
static struct color_ColorRGB matrix[4][12] = {0};

void led_Init(void){
   HAL_StatusTypeDef stat;
   uint8_t data[63 + 10] = {};

   // disable SW shutdown
   data[0] = REG_SHUTDOWN;
   data[1] = 0x1;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, data, 2, 1000);
   iprintf("Stat = 0x%x\n", stat);

   // set enable bit and scalar on all channels
   for(int i = 0; i < 36; i++) {
      _EnableChannel(i, DIVISOR_4);
   }

   _ForceUpdate();

   // enable all channels
   data[0] = REG_GLOBAL_CONTROL;
   data[1] = 0x0;
   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, data, 2, 1000);
}

void led_ClearDisplay(void) {
   //TODO actually disable the channels to save power
   /*
   for(int i = 0; i < 36; i++) {
      led_SetChannel(i, 0);
   }
   */
   //TODO fix this
}

void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV color) {
   //TODO update the in memory matrix
   //TODO color conversion
   //TODO sanity checks!

   //void color_HSV2RGB(struct color_ColorHSV const *hsv, struct color_ColorRGB *rgb);
   //matrix[y][x] = color;
   color_HSV2RGB(&color, &matrix[y][x]);
}

void led_UpdateDisplay(void) {
   //TODO draw the entire matrix

   //TODO const somewhere
   uint16_t matrixLUT[] =     {GPIO_PIN_8, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
   GPIO_TypeDef * matrixLUTPort[] = {GPIOA, GPIOB, GPIOB, GPIOB};

   //iprintf("top of scan cycle\n");
   //TODO macro size
   int i, col;
   for(i = 0; i < 4; i++) {
      //TODO start DMA this col now, before the other code runs?

      //iprintf("col %d\n", i);

      //clear everything
      for(col = 0; col < 4; col++) {
         HAL_GPIO_WritePin(matrixLUTPort[col], matrixLUT[col], GPIO_PIN_SET);
      }

      //TODO write new data to controller for this col while everything is off in ONE ARRAY SEND
      _WriteRow(i);

      //TODO can we do this in line after the final PWM val?
      _ForceUpdate();

      //enable the col to show
      HAL_GPIO_WritePin(matrixLUTPort[i], matrixLUT[i], GPIO_PIN_RESET);

      //persis so the human can see it
      HAL_Delay(1);
      //HAL_DelayUS(10);
   }
}

static bool _WriteRow(int rowIndex) {
   HAL_StatusTypeDef stat;
   uint8_t config[36 + 1] = {REG_PWM_BASE, 0};

   //FIXME better way?
   memcpy(&config[1], matrix[rowIndex], 36);

   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 100);
   if(stat != 0) {
      iprintf("Stat = 0x%x\n", stat);
      return false;
   }
   return true;
}

/*
bool led_SetChannel(uint8_t chan, uint8_t intensity) {
   return _SetChannelRaw(chan, intensity);
}
*/

static bool _SetChannelRaw(uint8_t chan, uint8_t intensity) {
   //TODO on intensity 0, disable the channel?
   HAL_StatusTypeDef stat;
   uint8_t config[2] = {};

   config[0] = REG_PWM_BASE + chan;
   config[1] = intensity;

   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 1000);
   if(stat != 0) {
      iprintf("Stat = 0x%x\n", stat);
      return false;
   }

   //TODO do this more efficiently?
   _ForceUpdate();

   return true;
}

static bool _EnableChannel(uint8_t chan, enum led_Divisor div) {
   HAL_StatusTypeDef stat;
   uint8_t config[2] = {};

   config[0] = REG_LED_CONTROL_BASE + chan;
   // enable this channel, and set the current divisor
   config[1] = (0x1 << 0) | (div << 1);

   stat = HAL_I2C_Master_Transmit(&hi2c1, LED_CONT_ADDR, config, sizeof(config), 1000);
   iprintf("Stat = 0x%x\n", stat);
   return (stat == 0);
}

//update the contents of the display with what's in its memory
static bool _ForceUpdate(void) {
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
