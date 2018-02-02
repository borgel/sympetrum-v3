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

void led_test(void) {
   //control lines init by platform GPIO ini

   int i;
   for(i = 0; i < 36; i++) {
      iprintf("set %d\n", i);
      led_SetChannel(i, 30);
      //HAL_Delay(100);
   }

   iprintf("Matrix Scan...\n");

   //FIXME move
   //easy way to reference each matrix column
   //uint16_t matrixLUT[] = {GPIO_PIN_8, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_15};
   uint16_t matrixLUT[] =     {GPIO_PIN_8, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
   GPIO_TypeDef * matrixLUTPort[] = {GPIOA, GPIOB, GPIOB, GPIOB};

   int col;
   while(true)
   {
      //iprintf("top of scan cycle\n");
      for(i = 0; i < 4; i++) {
         iprintf("en %d\n", i);
         //scan all columns. Turn off everything except the one to show
         for(col = 0; col < 4; col++) {
            iprintf(" %d", col);
            if(col == i) {
               //active LOW enables
               HAL_GPIO_WritePin(matrixLUTPort[col], matrixLUT[col], GPIO_PIN_RESET);
            }
            else {
               HAL_GPIO_WritePin(matrixLUTPort[col], matrixLUT[col], GPIO_PIN_SET);
            }
         }
         iprintf("\n");
         //HAL_Delay(10);
      }
   }
}

int main(void)
{
   HAL_Init();

   platformHW_Init();

   iprintf("\r\nStarting... (v%d | #0x%x / 0x%x | Built "__DATE__":"__TIME__")\r\n", FW_VERSION, bid_GetID(), bid_GetIDCrc());

   //ir_InitDecode();
   //ir_InitEncode();
   led_Init();

   HAL_Delay(10);

   led_test();

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

