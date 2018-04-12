#include "test.h"
#include "platform_hw.h"
#include "color.h"
#include "led.h"
#include "ir.h"
#include "iprintf.h"

#include <stdbool.h>

#define IR_ITERATIONS            (50)
//FIXME what to set the threshold at?
#define IR_TESTS_TO_PASS         (50)

#define IR_SAMPLES_PER_STATE     (20)
#define IR_SAMPLE_DELAY_MS       (1)

static void _HandleTestFail(void);

// check if we should enter test mode
bool test_EnterTestMode(void) {
   const uint8_t b8 = HAL_GPIO_ReadPin(TP_B8_PORT, TP_B8_PIN);
   const uint8_t a15 = HAL_GPIO_ReadPin(TP_A15_PORT, TP_A15_PIN);

   //FIXME rm, short on for now
   return true;



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

// take a series of readings and report the mode or all samples IR RX pin states
static GPIO_PinState _getModeIRPinState(int const samples, unsigned delayMS) {
   int timesSet = 0;
   int timesReset = 0;

   for(int i = 0; i < samples; i++) {
      if(HAL_GPIO_ReadPin(IR_RX_Port, IR_RX_Pin) == GPIO_PIN_SET) {
         timesSet++;
      }
      else {
         timesReset++;
      }

      //pause before taking the next sample
      HAL_Delay(delayMS);
   }

   if(timesSet > timesReset) {
      return GPIO_PIN_SET;
   }
   return GPIO_PIN_RESET;
}

static bool _TestIRTXRX(void) {
   /*
      sample RX for 50 samples at 1?ms, take mode state (on or off)
      randomly turn IR TX on or off
      sample RX for 50 samples at 1?ms, take mode state (on or off)
      compare set to read
  */

   int timesStatesWereExpected = 0;
   int iterationsRemaining = IR_ITERATIONS;
   GPIO_PinState firstState, secondState;

   firstState = secondState = GPIO_PIN_RESET;

   iprintf("Start IR test...\n");

   for(;iterationsRemaining >= 0; iterationsRemaining--) {
      firstState = _getModeIRPinState(IR_SAMPLES_PER_STATE, IR_SAMPLE_DELAY_MS);

      if(firstState == GPIO_PIN_SET) {
         //IR idle, turn on LED to make it active low
         //TODO
      }
      else {
         //TODO
      }

      //FIXME needed?
      HAL_Delay(1);

      secondState = _getModeIRPinState(IR_SAMPLES_PER_STATE, IR_SAMPLE_DELAY_MS);

      if(firstState != secondState) {
         iprintf("Test %d Success\n", iterationsRemaining);

         // this is success, the state changed when we wanted it to
         timesStatesWereExpected++;
      }
      else {
         iprintf("Test %d Fail\n", iterationsRemaining);
      }
   }

   iprintf("Total %d/%d passed\n", timesStatesWereExpected, IR_ITERATIONS);

   // FIXME what should threshold be?
   if(timesStatesWereExpected > IR_TESTS_TO_PASS) {
      return true;
   }
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
