#pragma once

#include <stdint.h>

void pattern_Init(void);
void pattern_Timeslice(uint32_t const timeMS);
void pattern_DoSendBeacon(void);

