#include "test.h"
#include "platform_hw.h"
#include "color.h"
#include "led.h"
#include "led_test.h"
#include "ir.h"
#include "ir_test.h"
#include "iprintf.h"

#include <stdbool.h>

#define IR_ITERATIONS            (50)
//FIXME what to set the threshold at?
#define IR_TESTS_TO_PASS         (50)

#define IR_SAMPLES_PER_STATE     (20)
#define IR_SAMPLE_DELAY_MS       (1)

#define LED_TEST_DELAY_MS        (2000)

#define INTER_TEST_DELAY_MS      (2000)

enum TestStatusLED {
   // no LEDs
   TS_NONE,
   // both LEDs
   TS_READY,
   // green (A5)
   TS_FAIL,
   // fail (A15)
   TS_PASS,
};
#define TP_ID_FAIL         (TP_A15)
#define TP_ID_PASS         (TP_A5)

union Interrupts {
   uint32_t mask;
   struct {
      uint8_t     userButton  : 1;
      uint8_t     tpA5        : 1;
      uint8_t     tpA15       : 1;
      uint8_t     tpB8        : 1;
   };
};
union Interrupts events = {0};

typedef bool (*TestFunction)(void * param);
struct TestPlanItem {
   TestFunction func;

   // if the next item should run automatically after this
   bool passthrough;

   void * param;
};

static bool TestModeActive = false;

static void _ShowColorOnBank(struct color_ColorRGB * c, int bank);
static void _SetTestStatusLEDs(enum TestStatusLED stat);
static void _SetTestpoint(enum TestPoints tp, bool set);
static bool _GetTestpoint(enum TestPoints tp);

// check if we should enter test mode
bool test_EnterTestMode(void) {
   // if user button pressed, enter test mode
   const bool b = HAL_GPIO_ReadPin(USER_BUTTON_PORT, USER_BUTTON_PIN) == GPIO_PIN_RESET;

   // strap b8 high and a15 low
   if(b) {
      return true;
   }
   return false;
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

static bool _TestIRTXRX(void * param) {
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

   iprintf("Start IR tests\n");

   for(;iterationsRemaining > 0; iterationsRemaining--) {
      firstState = _getModeIRPinState(IR_SAMPLES_PER_STATE, IR_SAMPLE_DELAY_MS);

      if(firstState == GPIO_PIN_SET) {
         //IR idle, turn on LED to make it active low
         ir_TestSetEnableTX(IR_TXE_ENABLE);
      }
      else {
         ir_TestSetEnableTX(IR_TXE_DISABLE);
      }

      //FIXME needed?
      HAL_Delay(1);

      secondState = _getModeIRPinState(IR_SAMPLES_PER_STATE, IR_SAMPLE_DELAY_MS);

      if(firstState != secondState) {
         iprintf(".");

         // this is success, the state changed when we wanted it to
         timesStatesWereExpected++;
      }
      else {
         iprintf("F");
      }
   }

   iprintf("\n");
   iprintf("Total %d/%d passed\n", timesStatesWereExpected, IR_ITERATIONS);

   // make sure this turns back off
   ir_TestSetEnableTX(IR_TXE_DISABLE);

   // FIXME what should threshold be?
   if(timesStatesWereExpected >= IR_TESTS_TO_PASS) {
      // show success
      struct color_ColorRGB g = {.g = 255};
      _ShowColorOnBank(&g, 2);

      return true;
   }

   // show fail
   struct color_ColorRGB r = {.r = 255};
   _ShowColorOnBank(&r, 2);

   return false;
}

static void _ShowColorOnBank(struct color_ColorRGB * c, int bank) {
   led_TestExEnableBank(bank);

   for(int led = 0; led < MATRIX_COLS; led++) {
      led_TestDrawPixel(bank, led, c);
   }
   led_TestRefresh(bank);
}

/*
// show the given color on all displays rows sequentially
static void _ShowColorOnRows(struct color_ColorRGB * c) {
   struct color_ColorRGB black = {0};
   //display is 12 x 4
   for(int bank = LED_TBANK_START; bank < LED_TBANK_END; bank++) {
      led_TestExEnableBank(bank);

      for(int led = 0; led < 12; led++) {
         led_TestDrawPixel(bank, led, c);
      }
      led_TestRefresh(bank);

      HAL_Delay(LED_TEST_DELAY_MS);

      // turn off this bank
      for(int led = 0; led < 12; led++) {
         led_TestDrawPixel(bank, led, &black);
      }
   }

   HAL_Delay(LED_TEST_DELAY_MS);
}

static void _TestLEDs(void) {
   struct color_ColorRGB r = {.r = 255};
   struct color_ColorRGB g = {.g = 255};
   struct color_ColorRGB b = {.b = 255};

   iprintf("Start LED Tests: ");

   iprintf("R");
   _ShowColorOnRows(&r);
   iprintf("G");
   _ShowColorOnRows(&g);
   iprintf("B");
   _ShowColorOnRows(&b);
   iprintf("\n");
}
*/

static bool _TestLED_R(void* param) {
   struct color_ColorRGB r = {.r = 255};
   _ShowColorOnBank(&r, (int)param);
   // can't fail
   return true;
}
static bool _TestLED_G(void* param) {
   struct color_ColorRGB g = {.g = 255};
   _ShowColorOnBank(&g, (int)param);
   // can't fail
   return true;
}
static bool _TestLED_B(void* param) {
   struct color_ColorRGB b = {.b = 255};
   _ShowColorOnBank(&b, (int)param);
   // can't fail
   return true;
}

static bool _ResetState(void* param) {
   // disable all LEDs
   struct color_ColorRGB c = {.r = 0, .g = 0, .b = 0};
   _ShowColorOnBank(&c, 0);

   // use test LEDs to show we are idle/ready
   _SetTestStatusLEDs(TS_READY);

   return true;
}

static struct TestPlanItem const TestPlan[] = {
   { _ResetState, false, NULL},

   //TODO there's gotta be a better way to do this
   { _TestIRTXRX, false, NULL},

   // for LEDs, pass in the bank
   { _TestLED_R, false, (void*)0},
   { _TestLED_R, false, (void*)1},
   { _TestLED_R, false, (void*)2},
   { _TestLED_R, false, (void*)3},

   { _TestLED_G, false, (void*)0},
   { _TestLED_G, false, (void*)1},
   { _TestLED_G, false, (void*)2},
   { _TestLED_G, false, (void*)3},

   { _TestLED_B, false, (void*)0},
   { _TestLED_B, false, (void*)1},
   { _TestLED_B, false, (void*)2},
   { _TestLED_B, false, (void*)3},
};
static int currentItem = 0;

void test_DoTests(void) {
   // mark us as having entered test mode
   TestModeActive = true;

   static int const TestPlanSize = sizeof(TestPlan) / sizeof(TestPlan[0]);
   iprintf("Starting %d Self Tests...\n", TestPlanSize);

   // test init
   ir_TestInit();
   led_TestInit();

   // start the 0th test by default
   currentItem = 0;

   bool testResult;
   bool lastTestWasPassthrough = false;
   int completedTestplanIterations = 0;

   // if first test is passthrough, run it
   lastTestWasPassthrough = TestPlan[currentItem].passthrough;

   while(true) {
      if(events.mask || lastTestWasPassthrough) {
         if(events.userButton) {
            iprintf("User Button\n");
            events.userButton = 0;
         }
         else if(events.tpB8) {
            iprintf("B8\n");
            events.tpB8 = 0;
         }

         //FIXME do none?
         _SetTestStatusLEDs(TS_READY);

         testResult = TestPlan[currentItem].func(TestPlan[currentItem].param);
         if(testResult == true) {
            iprintf("Test Pass\n");
            _SetTestStatusLEDs(TS_PASS);
         }
         else {
            iprintf("Test Fail\n");
            _SetTestStatusLEDs(TS_FAIL);
         }

         // track if the next test should run automatically
         lastTestWasPassthrough = TestPlan[currentItem].passthrough;
         if(lastTestWasPassthrough) {
            iprintf("Passthrough\n");
         }

         currentItem++;
         if(currentItem >= TestPlanSize) {
            currentItem = 0;

            completedTestplanIterations++;

            iprintf("Completed %d test plan iterations\n", completedTestplanIterations);
         }
      }

      //TODO on one complete cycle, terminate?
   }
}

// handle a button press from main
void test_UserButton(bool const buttonPressed) {
   if(!TestModeActive) {
      return;
   }

   // falling edge
   if(!buttonPressed) {
      events.userButton = 1;
   }
}

// handle a TP button press
void test_DoTPButton(enum TestPoints tp, bool const buttonPressed) {
   if(!TestModeActive) {
      return;
   }

   switch(tp) {
      case TP_B8:
         // rising edge
         if(buttonPressed) {
            events.tpB8 = 1;
         }
         break;

      case TP_A5:
      case TP_A15:
      default:
         break;
   }
}

static void _SetTestStatusLEDs(enum TestStatusLED stat) {
   bool passLED;
   bool failLED;

   switch(stat) {
      case TS_NONE:
         passLED = false;
         failLED = true;
         break;

      case TS_READY:
         passLED = true;
         failLED = true;
         break;

      case TS_PASS:
         passLED = true;
         failLED = false;
         break;

      case TS_FAIL:
         passLED = false;
         failLED = true;
         break;
   }

   _SetTestpoint(TP_ID_PASS, passLED);
   _SetTestpoint(TP_ID_FAIL, failLED);
}

// Set the given TP to the given level
static void _SetTestpoint(enum TestPoints tp, bool set) {
   switch(tp) {
      case TP_A5:
         HAL_GPIO_WritePin(TP_A5_PORT, TP_A5_PIN, set ? GPIO_PIN_SET : GPIO_PIN_RESET);
         break;

      case TP_A15:
         HAL_GPIO_WritePin(TP_A15_PORT, TP_A15_PIN, set ? GPIO_PIN_SET : GPIO_PIN_RESET);
         break;

      case TP_B8:
         HAL_GPIO_WritePin(TP_B8_PORT, TP_B8_PIN, set ? GPIO_PIN_SET : GPIO_PIN_RESET);
         break;
   }
}

static bool _GetTestpoint(enum TestPoints tp) {
   switch(tp) {
      case TP_A5:
         return HAL_GPIO_ReadPin(TP_A5_PORT, TP_A5_PIN) == GPIO_PIN_SET;

      case TP_A15:
         return HAL_GPIO_ReadPin(TP_A15_PORT, TP_A15_PIN) == GPIO_PIN_SET;

      case TP_B8:
         return HAL_GPIO_ReadPin(TP_B8_PORT, TP_B8_PIN) == GPIO_PIN_SET;
   }
   // should never run
   return false;
}

