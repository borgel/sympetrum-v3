#include "terrible_timer.h"

#include "stm32f0xx_hal.h"

#include "iprintf.h"

#include <stdlib.h>
#include <stdbool.h>

void ttimer_Reset(struct TerribleTimer* t) {
   t->durationMS = 0;
   t->startMS = 0;
   t->pendingComplete = false;
   t->repeat= false;
}

void ttimer_Set(struct TerribleTimer* t, bool restartWhenDone, bool restartNow, uint32_t const durationMS) {
   t->repeat = restartWhenDone;
   t->durationMS = durationMS;
   if(restartNow) {
      ttimer_Restart(t);
   }
}

// adjusts the current time (passing 10ms makes the clock jump 10ms into the future_
void ttimer_Adjust(struct TerribleTimer* const t, int32_t adjustment) {
   // adjust the timer by adjusting it's start point
   t->startMS += adjustment;
}

void ttimer_Restart(struct TerribleTimer* t) {
   //FIXME pass this in? set a CB on init?
   t->startMS = HAL_GetTick();
   t->pendingComplete = false;
}

bool ttimer_HasElapsed(struct TerribleTimer* const t) {
   // only return EDGES
   if(t->pendingComplete) {
      return true;
   }

   //FIXME rm
   //iprintf("%d - %d = %d ( > %d)\n", HAL_GetTick(), t->start, (HAL_GetTick() - t->start), t->duration);

   if(HAL_GetTick() - t->startMS > t->durationMS) {
      t->pendingComplete = true;
      // restart the timer if it's going to repeat
      if(t->repeat) {
         ttimer_Restart(t);
      }
      return true;
   }
   return false;
}

