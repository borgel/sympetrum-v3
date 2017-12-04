#pragma once

#include <stdint.h>
#include <stdbool.h>

void led_Init(void);
bool led_SetChannel(uint8_t chan, uint8_t intensity);
void led_ClearDisplay(void);

