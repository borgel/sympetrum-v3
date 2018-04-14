/*
 * Only to be used for testing LEDs, not in normal operation!
 */
#pragma once

#include <stdint.h>

#include "color.h"

enum led_TestBankID {
   LED_TBANK_START = 0,
   LED_TBANK_0 = LED_TBANK_START,
   LED_TBANK_1,
   LED_TBANK_2,
   LED_TBANK_3,
   LED_TBANK_END,
};

void led_TestInit(void);
void led_TestExEnableBank(enum led_TestBankID bank);
void led_TestDrawPixel(uint8_t x, uint8_t y, struct color_ColorRGB * color);
void led_TestRefresh(enum led_TestBankID bank);

