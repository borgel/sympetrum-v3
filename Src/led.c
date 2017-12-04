#include "led.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "iprintf.h"

#include <stdint.h>

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
   for(int i = 0; i < 36; i++) {
      led_SetChannel(i, 0);
   }
}

bool led_SetChannel(uint8_t chan, uint8_t intensity) {
   uint8_t mapped;

   /*
    * we need to remap the logical channels to physical. It turns out that can
    * be done in a simple (cheesy) way by moving the top half to the bottom, and
    * the bottom half to the top.
    * Channel 0 is the top (far from the uUSB port)
    */
   if(chan < 18) {
      mapped = chan + 18;
   }
   else {
      mapped = chan - 18;
   }
   return _SetChannelRaw(mapped, intensity);
}

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
