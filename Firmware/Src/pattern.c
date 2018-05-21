#include "pattern.h"

#include "ir.h"
#include "terrible_timer.h"
#include "iprintf.h"

#include <stdint.h>

// the UNCHANGING beacon clock period
#define  BEACON_CLOCK_DEFAULT_PERIOD_MS       (60 * 1000)

struct State {
   uint32_t lastUpdateMS;

   // slow clock that drives everything else
   struct TerribleTimer beaconClock;
   //TODO name?
   //fast clock that regulates the animation in progress
   struct TerribleTimer animationClock;
};
static struct State state = {0};

void pattern_Init(void) {
   IRInit();

   ttimer_Set(&state.beaconClock, true, BEACON_CLOCK_DEFAULT_PERIOD_MS);
   //TODO start anim clock?

   iprintf("Pattern Init Complete...\n");
}

void pattern_Timeslice(uint32_t const timeMS) {
   // check for new messages
   if(IRDataReady()) {
      iprintf("Got Incoming IR Message ");

      uint32_t bytes = 0;
      uint8_t* buf = IRGetBuff(&bytes);
      iprintf("%d bytes: [%s]\n", bytes, (char*)buf);

      //TODO do something clocky
   }

   if(ttimer_HasElapsed(&state.beaconClock)) {
      iprintf("Beacon!\n");
   }

   // TODO ? check for anum clock?
}
