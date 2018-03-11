#include "test.h"
#include "platform_hw.h"
#include "color.h"
#include "led.h"
#include "iprintf.h"

#include <stdbool.h>

// check if we should enter test mode
bool test_EnterTestMode(void) {
   const uint8_t b8 = HAL_GPIO_ReadPin(TP_B8_PORT, TP_B8_PIN);
   const uint8_t a15 = HAL_GPIO_ReadPin(TP_A15_PORT, TP_A15_PIN);

   // strap b8 high and a15 low
   if(b8 == GPIO_PIN_SET && a15 == GPIO_PIN_RESET) {
      return true;
   }
   return false;
}

static bool _TestALS(void) {
   //TODO sanity check ALS (non 0, non saturated)

   // we can't really check for sure, so pretend it's fine
   return true;
}

static bool _TestButtons(void) {
   //TODO just print states?
   //check if idle state is what's expected
}

static bool _TestIRTXRX(void) {
   //TODO send an IR msg and verify we got SOMETHING back
}

// show the given color on all displays rows sequentially
static void _ShowColorOnRows(struct color_ColorHSV * c) {
   //display is 12 x 4
   // set each row to R, G, B
   for(int row = 0; row < 4; row++) {
      //set this entire row
      for(int col = 0; col < 12; col++) {
         led_DrawPixel(row, col, *c);
      }
      HAL_Delay(1250);
   }
}

static void _TestLEDs(void) {
   struct color_ColorHSV r = {.h = 0, .s = 255, .v = 255};
   struct color_ColorHSV g = {.h = 85, .s = 255, .v = 255};
   struct color_ColorHSV b = {.h = 170, .s = 255, .v = 255};

   _ShowColorOnRows(&r);
   _ShowColorOnRows(&g);
   _ShowColorOnRows(&b);
}

void test_DoTests(void) {
   while(true) {
      _TestALS();
      _TestButtons();
      _TestIRTXRX();
      _TestLEDs();

      HAL_Delay(1000);
   }
}

