#include "terrible_timer.h"

#include "stm32f0xx_hal.h"

#include "iprintf.h"

#include <stdlib.h>
#include <stdbool.h>

void ttimer_Reset(struct TerribleTimer* t) {
   t->durationMS = 0;
   t->startMS = 0;
   t->pendingComplete = true;
   t->repeat= false;
}

void ttimer_Set(struct TerribleTimer* t, bool restartWhenDone, uint32_t const durationMS) {
   t->repeat = restartWhenDone;
   t->durationMS = durationMS;
   ttimer_Restart(t);
}

void ttimer_Restart(struct TerribleTimer* t) {
   //FIXME pass this in?
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

