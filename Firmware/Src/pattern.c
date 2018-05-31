#include "pattern.h"

#include "ir.h"
#include "terrible_timer.h"
#include "lighting.h"
#include "iprintf.h"

#include "color.h"

#include <stdint.h>

// the UNCHANGING beacon clock period
//#define  BEACON_CLOCK_DEFAULT_PERIOD_MS         (120 * 1000)
#define  BEACON_CLOCK_DEFAULT_PERIOD_MS         (30 * 1000)

#define  MAX_JITTER                             (255)

extern uint8_t const CosTable[];
extern uint8_t const CosTableSize;

//FIXME package into TA struct
#define  ANIMATION_FRAMES                       (CosTableSize)

struct InteractionRamp {
   // divisor to the beacon clock to apply. larger numbers animate faster
   uint8_t     clockDivisor;
   // max allowed jitter. 255 will look random
   uint8_t     maxJitter;
};
// the behavioral ramps that govern how the unit behaves when it sees friends
static const struct InteractionRamp const interactionRamp[] = {
   // divisor, maxJtter
   // starting activity level
   {1,   MAX_JITTER},
   // one other device recently seen
   {13,   30},
   // many devices seen
   {40,  0},
};
static int const interactionRampLength = sizeof(interactionRamp) / sizeof(interactionRamp[0]);

// used to travel up and down the interaction ramp
enum InteractionRampChoice {
   IRC_Increment,
   IRC_Decrement,
};

struct TerribleAnimation {
   //TODO package these into a module somewhere
   uint8_t frame;
   uint8_t clockDivisor;
   // TODO use?
   //uint8_t period;

   uint8_t maxJitter;

   uint8_t huePhase;
};

struct State {
   uint32_t lastUpdateMS;

   // where on the interaction ramp we are
   uint8_t  rampPosition;

   struct TerribleAnimation animation;

   // slow clock that drives everything else
   struct TerribleTimer beaconClock;
   //fast clock that regulates the animation in progress
   struct TerribleTimer animationClock;
};
static struct State state = {0};

static void applyRampState(enum InteractionRampChoice const irc);
static void handleAnimationFrame(struct TerribleAnimation * const a);
static uint32_t getAnimationClockPeriod(struct TerribleAnimation const * const st);

void pattern_Init(void) {
   // set timers and config animation based on ramp position (start at min)
   applyRampState(IRC_Decrement);

   // setup the timers
   ttimer_Set(&state.beaconClock, true, BEACON_CLOCK_DEFAULT_PERIOD_MS);

   //FIXME rm
   iprintf("anim frame len is %d ms\n", getAnimationClockPeriod(&state.animation));

   // safe to init IR now that animation etc is setup
   IRInit();

   iprintf("Pattern Init Complete...\n");
}

void pattern_DoSendBeacon(void) {
   //TODO IR send a beacon

   //FIXME rm
   applyRampState(IRC_Increment);
}

void pattern_Timeslice(uint32_t const timeMS) {
   // check for new messages
   if(IRDataReady()) {
      iprintf("Got Incoming IR Message ");

      uint32_t bytes = 0;
      uint8_t* buf = IRGetBuff(&bytes);
      iprintf("%d bytes: [%s]\n", bytes, (char*)buf);

      //TODO reset both clocks? or kick forward?
   }

   if(ttimer_HasElapsed(&state.beaconClock)) {
      iprintf("Beacon!\n");

      //FIXME rm
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
   //FIXME rm
   iprintf("F%d ", a->frame);

   const uint32_t duration = getAnimationClockPeriod(a);

   applyAnimationFrame(a->frame, duration, a->huePhase, a->maxJitter);

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

static void applyRampState(enum InteractionRampChoice const irc) {
   // apply the settings in this level of the interaction ramp to the animations
   iprintf("do ramp\n");

   //FIXME rm
   iprintf("ramp from %d to ", state.rampPosition);

   // resolve ramp movement first
   if(irc == IRC_Increment) {
      if(state.rampPosition + 1 < interactionRampLength) {
         state.rampPosition++;
      }
   }
   else {
      if(state.rampPosition > 0) {
         state.rampPosition--;
      }
   }
   //FIXME rm
   iprintf("%d\n", state.rampPosition);

   struct InteractionRamp const * const r = &interactionRamp[state.rampPosition];
   struct TerribleAnimation * const ta = &state.animation;

   // set max jitter in anim in state
   ta->maxJitter = r->maxJitter;

   // update duration
   ta->clockDivisor = r->clockDivisor;
   ttimer_Set(&state.animationClock, true, getAnimationClockPeriod(&state.animation));

   iprintf("anim frame len is %d ms\n", getAnimationClockPeriod(&state.animation));
}

