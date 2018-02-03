#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "color.h"

void led_Init(void);
//FIXME rm
//bool led_SetChannel(uint8_t chan, uint8_t intensity);
void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV color);

void led_ClearDisplay(void);

// Pump frequently to draw the matrix
void led_UpdateDisplay(void);
// call to complete an entire draw cycle immediately
void led_ForceRefresh(void);

