#include "lighting.h"

#include "led.h"
#include "als.h"
#include "iprintf.h"

#include <stdint.h>

#define ALS_POLL_INTERVAL_MS        (10 * 1000)

struct State {
   uint32_t alsInProgress;
   uint32_t lastALS;
};
static struct State state = {0};

void lighting_Init(void) {
   led_Init();
   als_Init();

   // TODO take and handle one ALS sample

   //TODO coordinate animations
}

void lighting_Timeslice(uint32_t const timeMS) {
   //TODO ALS
   if(timeMS - state.lastALS > ALS_POLL_INTERVAL_MS) {
      if(!state.alsInProgress) {
         state.alsInProgress = true;
         als_StartReading();
         return;
      }

      uint32_t lux;
      if(als_GetLux(&lux)) {
         // we got a valid reading. Reset things
         state.alsInProgress = false;
         state.lastALS = timeMS;

         // FIXME rm
         iprintf("Light Counts = %d\n", lux);

         //TODO act on ALS reading
      }
   }
}
