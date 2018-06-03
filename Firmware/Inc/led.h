#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "color.h"

enum led_Brightness {
   LED_DIV_1,     //bright
   LED_DIV_2,
   LED_DIV_3,
   LED_DIV_4,     //dim
};

void led_Init(void);
void led_MatrixStart(void);
void led_MatrixStop(void);
void led_SetGlobalBrightness(enum led_Brightness bright, uint8_t sub);

void led_DrawPixelLinear(uint8_t x, struct color_ColorHSV * const color);

void led_ClearDisplay(void);

// pause display for a moment so other ISR things can happen
void led_Pause(void);
void led_Resume(void);

// Pump frequently to draw the matrix
void led_UpdateDisplay(void);
// call to complete an entire draw cycle immediately
void led_ForceRefresh(void);

