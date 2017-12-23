/**
 */
#include "platform_hw.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_hal_tim.h"
#include "iprintf.h"

#include "led.h"
#include "board_id.h"
#include "version.h"
#include "ir_encode.h"
#include "ir_decode.h"

#include <string.h>
#include <stdlib.h>

union Interrupts {
   uint32_t mask;
   struct {
      uint8_t     accelerometer  : 1;
      uint8_t     charger        : 1;
   };
};

int main(void)
{
   HAL_Init();

   platformHW_Init();

   iprintf("\r\nStarting... (v%d | #0x%x / 0x%x | Built "__DATE__":"__TIME__")\r\n", FW_VERSION, bid_GetID(), bid_GetIDCrc());

   ir_InitDecode();
   ir_InitEncode();
   led_Init();

   HAL_Delay(10);

   led_SetChannel(3, 130);

   ir_DecodeEnable();

   uint16_t rxData;

   while(1) {
      ir_SendRaw(0x318);
      if(ir_GetDecoded(&rxData, NULL)) {
         iprintf("Got Packet: 0x%x\n", rxData);
         rxData = 0;
      }
   }

   return 0;
}

