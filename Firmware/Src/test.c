#include "test.h"
#include "platform_hw.h"
#include "color.h"
#include "led.h"
#include "iprintf.h"

#include <stdbool.h>

static void _HandleTestFail(void);

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

// check if idle state is what's expected
static bool _TestButtons(void) {
   // we expect user button to float high
   if(HAL_GPIO_ReadPin(USER_BUTTON_PORT, USER_BUTTON_PIN) != GPIO_PIN_SET) {
      iprintf("user button was not asserted\n");
      return false;
   }
   return true;
}

static bool _TestIRTXRX(void) {
   //TODO send an IR msg and verify we got SOMETHING back

   //FIXME rm?
   return false;
}

// show the given color on all displays rows sequentially
static void _ShowColorOnRows(struct color_ColorHSV * c) {
   //display is 12 x 4
   // set each row to R, G, B
   for(int row = 0; row < 12; row++) {
      //set this entire row
      for(int col = 0; col < 4; col++) {
         led_DrawPixel(row, col, c);
      }
   }
   HAL_Delay(1250);
}

static void _TestLEDs(void) {
   //TODO figure out real values for R/G/B
   struct color_ColorHSV r = {.h = HSV_COLOR_R, .s = 255, .v = 255};
   struct color_ColorHSV g = {.h = HSV_COLOR_G, .s = 255, .v = 255};
   struct color_ColorHSV b = {.h = HSV_COLOR_B, .s = 255, .v = 255};

   iprintf("R");
   _ShowColorOnRows(&r);
   iprintf("G");
   _ShowColorOnRows(&g);
   iprintf("B");
   _ShowColorOnRows(&b);
   iprintf(" ");
}

void test_DoTests(void) {
   iprintf("Starting Self Tests...\n");

   // test init
   led_SetGlobalBrightness(255);

   while(true) {
      if(!_TestALS()) {
         _HandleTestFail();
      }

      if(!_TestButtons()) {
         _HandleTestFail();
      }

      if(!_TestIRTXRX()) {
         _HandleTestFail();
      }

      _TestLEDs();

      HAL_Delay(1000);
   }
}

static void _HandleTestFail(void) {
   iprintf("Terminal test failure\n");
   while(true) {}
}
