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

void test_DoTests(void) {
   //TODO run tests
}

