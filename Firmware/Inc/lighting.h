#pragma once

#include "color.h"

#include <stdint.h>

void lighting_Init(void);
void lighting_Timeslice(uint32_t const timeMS);

void lighting_DrawPixelLinear(uint8_t x, struct color_ColorHSV * const color);
void lighting_DrawRing(uint8_t r, struct color_ColorHSV * const color);

