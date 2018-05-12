#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "color.h"

void led_Init(void);
void led_MatrixStart(void);
void led_MatrixStop(void);
void led_SetGlobalBrightness(uint8_t bright);

void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV * color);
void led_DrawPixelLinear(uint8_t x, struct color_ColorHSV * const color);
void led_DrawSparse(uint8_t x, uint8_t y, struct color_ColorHSV * color);

void led_ClearDisplay(void);

// pause display for a moment so other ISR things can happen
void led_Pause(void);
void led_Resume(void);

// Pump frequently to draw the matrix
void led_UpdateDisplay(void);
// call to complete an entire draw cycle immediately
void led_ForceRefresh(void);

