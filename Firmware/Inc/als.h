#pragma once

#include <stdint.h>
#include <stdbool.h>

enum als_LightCondition {
   ALC_Start,
   ALC_IndoorDark,
   ALC_IndoorLight,
   ALC_OutdoorShade,
   ALC_Sunlight,
   ALC_End,
};

bool als_Init(void);
void als_StartReading(void);
bool als_GetLux(enum als_LightCondition * cond);

