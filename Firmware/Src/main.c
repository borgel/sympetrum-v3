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
#include "ir.h"

#include <string.h>
#include <stdlib.h>

union Interrupts {
   uint32_t mask;
   struct {
      uint8_t     accelerometer  : 1;
      uint8_t     charger        : 1;
   };
};

static void testSympIR(void) {
   ir_InitDecode();
   ir_InitEncode();

   ir_DecodeEnable();

   uint16_t rxData;

   while(1) {
      ir_SendRaw(0x318);
      if(ir_GetDecoded(&rxData, NULL)) {
         iprintf("Got Packet: 0x%x\n", rxData);
         rxData = 0;
      }
   }
}

static void testDarknetIR(void) {
   iprintf("Darknet TX\n");

   IRInit();

   iprintf("dnet IR init done\n");

   uint8_t buf[] = "Test Str";

   iprintf("TX...");
   IRTxBuff(buf, sizeof(buf) - 1);
   iprintf("done\n");
   iprintf("have %d bytes\n", IRBytesAvailable());

   /*
   while(1) {
      iprintf("TX...");
      IRTxBuff(buf, sizeof(buf));
      iprintf("done\n");

      HAL_Delay(1000);

      if(IRBytesAvailable() > 0) {
         iprintf("have %d bytes\n", IRBytesAvailable());
      }
   }
   */
}

static void testDarknetRX(void) {
   iprintf("Darknet RX\n");
   IRInit();

   IRStartRx();

   int32_t bytes = 0;
   while(1) {
      bytes = IRBytesAvailable();
      if(bytes) {
         iprintf("have %d bytes\n", bytes);
         bytes = 0;
      }
      if(IRDataReady()) {
         iprintf("Got Full %d Byte Message! [", bytes);

         iprintf("%s", (char*)IRGetBuff());

         iprintf("]\n");

         IRStopRX();
         IRStartRx();
      }
   }
}

void led_test(void) {
   //control lines init by platform GPIO ini

   /*
   int i;
   for(i = 0; i < 36; i++) {
      iprintf("set %d\n", i);
      led_SetChannel(i, 30);
      //HAL_Delay(100);
   }
   */

   iprintf("Matrix Scan...\n");

   /*
   struct color_ColorHSV c = {.h = 10, .s = 255, .v = 255};
   struct color_ColorHSV c2 = {.h = 100, .s = 255, .v = 255};
   struct color_ColorHSV c3 = {.h = 200, .s = 255, .v = 255};

   led_DrawPixel(5, 2, c);
   led_DrawPixel(10, 0, c2);
   led_DrawPixel(11, 1, c3);
   */

   struct color_ColorHSV c = {.h = 10, .s = 255, .v = 255};
   int count = 0;
   int x, y;
   uint8_t off = 0;
   while(true)
   {
      //TODO permute pattern in memory
      //if(count >= 2)
      {
         count = 0;

         //permute
         for(y = 0; y < 4; y++) {
            for(x = 0; x < 12; x++) {
               c.h = (x * 5) + (y * 8) + off;
               led_DrawPixel(x, y, c);
            }
         }
         off++;
      }

      //draw it
      //led_ForceRefresh();

      count++;
  }
}

int main(void)
{
   HAL_Init();

   platformHW_Init();

   iprintf("\r\nStarting... (v%d | #0x%x / 0x%x | Built "__DATE__":"__TIME__")\r\n", FW_VERSION, bid_GetID(), bid_GetIDCrc());

   testDarknetIR();
   testDarknetRX();

   return 0;

   led_Init();
   HAL_Delay(10);
   
   //led_test();

   //testSympIR();

   return 0;
}

