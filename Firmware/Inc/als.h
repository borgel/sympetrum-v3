#pragma once

#include <stdint.h>
#include <stdbool.h>

bool als_Init(void);
void als_StartReading(void);
bool als_GetLux(uint32_t * lux);

