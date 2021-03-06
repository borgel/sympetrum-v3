#pragma once

#include <stdbool.h>
#include <stdint.h>

struct TerribleTimer;

// NOTE: Do not use directly, use APIs to interact
struct TerribleTimer {
   bool repeat;

   uint32_t durationMS;
   uint32_t startMS;
   bool pendingComplete;
};

// configure and start a timer
void ttimer_Set(struct TerribleTimer* t, bool restartWhenDone, bool restartNow, uint32_t const durationMS);

// apply a relative adjustment to a timer (running or not)
void ttimer_Adjust(struct TerribleTimer* const t, int32_t adjustment);

// reset and stop a timer
void ttimer_Reset(struct TerribleTimer* t);

// immediately restart a timer that is already configured, even if it is in progress
void ttimer_Restart(struct TerribleTimer* t);

// check if a timer has elapsed. If it resets, 
bool ttimer_HasElapsed(struct TerribleTimer* const t);
