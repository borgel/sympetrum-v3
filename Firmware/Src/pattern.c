#include "pattern.h"

#include "ir.h"
#include "terrible_timer.h"
#include "lighting.h"
#include "iprintf.h"

#include "color.h"

#include <stdint.h>

// the UNCHANGING beacon clock period
//#define  BEACON_CLOCK_DEFAULT_PERIOD_MS         (60 * 1000)
//FIXME rm
#define  BEACON_CLOCK_DEFAULT_PERIOD_MS         (60 * 1000)
#define  ANIMATION_CLOCK_DEFAULT_DIVISOR        (1)

extern uint8_t const CosTable[];
extern uint8_t const CosTableSize;

//FIXME package into TA struct
#define  ANIMATION_FRAMES                       (CosTableSize)

struct TerribleAnimation {
   //TODO package these into a module somewhere
   uint8_t frame;
   uint8_t clockDivisor;
   // TODO use?
   //uint8_t period;

   uint8_t huePhase;
};

struct State {
   uint32_t lastUpdateMS;

   struct TerribleAnimation animation;

   // slow clock that drives everything else
   struct TerribleTimer beaconClock;
   //TODO name?
   //fast clock that regulates the animation in progress
   struct TerribleTimer animationClock;
};
static struct State state = {.animation = {.clockDivisor = ANIMATION_CLOCK_DEFAULT_DIVISOR}};

static void handleAnimationFrame(struct TerribleAnimation * const a);
static uint32_t getAnimationClockPeriod(struct TerribleAnimation const * const st);

void pattern_Init(void) {
   IRInit();

   // setup the timers
   ttimer_Set(&state.beaconClock, true, BEACON_CLOCK_DEFAULT_PERIOD_MS);
   ttimer_Set(&state.animationClock, true, getAnimationClockPeriod(&state.animation));

   //FIXME rm
   iprintf("anim frame len is %d ms\n", getAnimationClockPeriod(&state.animation));

   iprintf("Pattern Init Complete...\n");
}

void pattern_Timeslice(uint32_t const timeMS) {
   // check for new messages
   if(IRDataReady()) {
      iprintf("Got Incoming IR Message ");

      uint32_t bytes = 0;
      uint8_t* buf = IRGetBuff(&bytes);
      iprintf("%d bytes: [%s]\n", bytes, (char*)buf);

      //TODO reset both clocks
   }

   if(ttimer_HasElapsed(&state.beaconClock)) {
      iprintf("Beacon!\n");
   }
   // pump the animation on a tick
   if(ttimer_HasElapsed(&state.animationClock)) {
      handleAnimationFrame(&state.animation);
   }
}

// FIXME package
static void applyAnimationFrame(uint8_t const frame, uint32_t durationMS, uint8_t phase, uint8_t maxJitter) {
   struct color_ColorHSV color = {.h = 0, .s = 255, .v = 255};
   for(int i = 0; i < 18; i++) {
      // adjust pitch? or just time between frames?
      // calculate what % done with the animation we are, then rescale it over table size
      //FIXME can we just use phase + frame?
      //int v = (((float)phase + (float)i * 1.0) / (float)ANIMATION_FRAMES) * (float)CosTableSize;
      // TODO adjust pitch. 4 is towards max
      int v = frame + phase + ((float)i * 2.0);
      v %= (int)CosTableSize;
      color.h = CosTable[v];

      lighting_DrawRing(i, &color, maxJitter, durationMS);
   }
}

static void handleAnimationFrame(struct TerribleAnimation * const a) {
   //TODO figure out this frame, and write it to YABI
   iprintf("F%d ", a->frame);

   const uint32_t duration = getAnimationClockPeriod(a);
   //TODO get max jitter from the ramp
   const uint8_t maxJitter = 0;

   applyAnimationFrame(a->frame, duration, a->huePhase, maxJitter);

   //TODO add bounded randomness (rand(255) - 128)? cast to u8 so it rolls?

   a->huePhase++;

   // cleanup state
   a->frame++;
   if(a->frame > ANIMATION_FRAMES) {
      a->frame = 0;
   }
}

static uint32_t getAnimationClockPeriod(struct TerribleAnimation const * const animation) {
   uint32_t const totalDuration = BEACON_CLOCK_DEFAULT_PERIOD_MS / animation->clockDivisor;

   return totalDuration / ANIMATION_FRAMES;
}

