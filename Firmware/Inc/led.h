#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "color.h"

void led_Init(void);
//FIXME rm
//bool led_SetChannel(uint8_t chan, uint8_t intensity);
void led_DrawPixel(uint8_t x, uint8_t y, struct color_ColorHSV color);

void led_ClearDisplay(void);

//FIXME trigger internally? on timer?
void led_UpdateDisplay(void);

