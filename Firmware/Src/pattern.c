#include "pattern.h"

#include "terrible_timer.h"
#include "lighting.h"
#include "iprintf.h"
#include "beacon.h"

#include "color.h"

#include <stdint.h>
#include <stdlib.h>

// the UNCHANGING beacon clock period
#define  BEACON_CLOCK_DEFAULT_PERIOD_MS         (30 * 1000)
#define  BEACON_CLOCK_BUMP_PERIOD               (-1 * (BEACON_CLOCK_DEFAULT_PERIOD_MS / 3))

#define  MAX_JITTER                             (255)

extern uint8_t const CosTable[];
extern uint8_t const CosTableSize;

//FIXME package into TerribleAnimation struct
#define  ANIMATION_FRAMES                       (CosTableSize)

struct InteractionRamp {
   // divisor to the beacon clock to apply. larger numbers animate faster
   uint8_t     clockDivisor;
   // multiplies frame duration by this to slow things down
   uint8_t     frameMultiplier;
   // max allowed jitter. 255 will look random
   uint8_t     maxJitter;
   // max rate that jitter should change to reach the target
   uint8_t     jitterSlew;
};
// the behavioral ramps that govern how the unit behaves when it sees friends
static const struct InteractionRamp const interactionRamp[] = {
   // divisor, maxJtter
   // starting activity level
   {1,  2, MAX_JITTER/2,  20},
   // one other device recently seen
   {3,  1, 30,             8},
   // many devices seen
   {12, 1,  0,             2},
   {12, 1,  0,             2},
   // there are two final levels so that we can go "above" fully synced. Otherwise we tend to
   // "bounce" against the top of the ramp and end up one slot short
};
static int const interactionRampLength = sizeof(interactionRamp) / sizeof(interactionRamp[0]);

// used to travel up and down the interaction ramp
enum InteractionRampChoice {
   IRC_Increment,
   IRC_Decrement,
};

struct TerribleAnimation {
   uint8_t frame;
   uint8_t clockDivisor;
   uint32_t frameLengthMS;

   uint8_t jitterSlew;
   uint8_t maxJitter;

   uint8_t huePhase;
};

struct State {
   uint32_t lastUpdateMS;

   // where on the interaction ramp we are
   uint8_t rampPosition;

   // the jitter we want to get to
   uint8_t jitterTarget;

   // the frame rate we want to get to. calculated via clock divisor
   uint32_t frameLengthTarget;

   struct TerribleAnimation animation;

   // slow clock that drives everything else
   struct TerribleTimer beaconClock;
   //fast clock that regulates the animation in progress
   struct TerribleTimer animationClock;
};
static struct State state = {0};

static void applyJitterChanges(void);
static void applyFrameLengthChanges(void);
static void applyRampState(enum InteractionRampChoice const irc);
static void handleAnimationFrame(struct TerribleAnimation * const a);
static uint32_t getAnimationClockPeriod(struct TerribleAnimation const * const st);

void pattern_Init(void) {
   // short circuit the ramp and set the correct starting jitter and frame duration
   state.animation.maxJitter = interactionRamp[0].maxJitter;

   // set timers and config animation based on ramp position (start at min)
   applyRampState(IRC_Decrement);

   // kick the animation clock
   state.animation.frameLengthMS = interactionRamp[0].frameMultiplier * getAnimationClockPeriod(&state.animation);
   ttimer_Set(&state.animationClock, true, true, state.animation.frameLengthMS);

   // setup the main timer
   ttimer_Set(&state.beaconClock, true, true, BEACON_CLOCK_DEFAULT_PERIOD_MS);

   // safe to init IR now that animation etc is setup
   beacon_Init();

   iprintf("Pattern Init Complete...\n");

   beacon_Send();
}

void pattern_DoSendBeacon(void) {
   beacon_Send();

   // animation briefly to show we are sending
   state.animation.maxJitter = 0;
}

void pattern_Timeslice(uint32_t const timeMS) {
   // check for new messages
   enum BeaconStatus bs = beacon_HaveReceived();
   if(bs != BS_None) {
      iprintf("Got an interesting beacon!\n");

      // TODO handle if it's special (BS_Special)

      iprintf("Bumped clock %d ms\n", BEACON_CLOCK_BUMP_PERIOD);

      // bump main clock forward 10%
      ttimer_Adjust(&state.beaconClock, BEACON_CLOCK_BUMP_PERIOD);

      applyRampState(IRC_Increment);
   }

   if(ttimer_HasElapsed(&state.beaconClock)) {
      iprintf("Send Beacon! ");

      pattern_DoSendBeacon();

      // decrement timer to keep the balance
      applyRampState(IRC_Decrement);
   }
   // pump the animation on a tick
   if(ttimer_HasElapsed(&state.animationClock)) {
      handleAnimationFrame(&state.animation);

      //TODO do this elsewhere?
      // progress jitter towards the target
      applyJitterChanges();

      // progress frame duration towards target
      applyFrameLengthChanges();
   }
}

static void applyFrameLengthChanges(void) {
   uint32_t const * const target = &state.frameLengthTarget;
   uint32_t * const current = &state.animation.frameLengthMS;
   uint8_t const slew = state.animation.jitterSlew;

   if(*current == *target) {
      return;
   }

   // if we are close, snap to the right value to avoid hunting forever
   if(abs(*current - *target) <= slew) {
      iprintf("Frame Length Slew Complete\n");
      *current = *target;
   }
   else {
      if(*current > *target) {
         (*current) -= slew;
      }
      else if(*target > *current) {
         (*current) += slew;
      }
   }

   // make sure the clock is correctly set
   ttimer_Set(&state.animationClock, true, false, *current);
}

static void applyJitterChanges(void) {
   uint8_t const * const target = &state.jitterTarget;
   uint8_t * const current = &state.animation.maxJitter;
   uint8_t const slew = state.animation.jitterSlew;

   if(*current == *target) {
      return;
   }

   // if we are close, snap to the right value to avoid hunting forever
   if(abs(*current - *target) <= slew) {
      iprintf("Jitter Slew Complete\n");
      *current = *target;
   }
   else {
      if(*current > *target) {
         (*current) -= slew;
      }
      else if(*target > *current) {
         (*current) += slew;
      }
   }
}

static void applyAnimationFrame(uint8_t const frame, uint32_t durationMS, uint8_t phase, uint8_t maxJitter) {
   struct color_ColorHSV color = {.h = 0, .s = 255, .v = 255};

   uint32_t v;
   for(int i = 0; i < 18; i++) {

      // TODO adjust pitch with speed. 9 is towards max
      v = frame + phase + ((float)i * 4.0);
      v %= CosTableSize;
      color.h = CosTable[v];

      lighting_DrawRing(i, &color, maxJitter, durationMS);
   }
}

static void handleAnimationFrame(struct TerribleAnimation * const a) {
   applyAnimationFrame(a->frame, a->frameLengthMS, a->huePhase, a->maxJitter);

   a->huePhase++;
   if(a->huePhase >  ANIMATION_FRAMES) {
      a->huePhase = 0;
   }

   // cleanup state
   a->frame++;
   if(a->frame > ANIMATION_FRAMES) {
      iprintf("A ");
      a->frame = 0;
   }
}

static uint32_t getAnimationClockPeriod(struct TerribleAnimation const * const animation) {
   uint32_t const totalDuration = BEACON_CLOCK_DEFAULT_PERIOD_MS / animation->clockDivisor;

   return totalDuration / ANIMATION_FRAMES;
}

static void applyRampState(enum InteractionRampChoice const irc) {
   // apply the settings in this level of the interaction ramp to the animations
   iprintf("Ramp from %d -> ", state.rampPosition);

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
   iprintf("%d\n", state.rampPosition);

   // centered random 500ms offset
   int littleBump = rand() % 750;
   littleBump -= 750;

   iprintf("Adding %d ms clock jitter\n", littleBump);

   // shake the clock up a little to prevent sync lock
   ttimer_Adjust(&state.beaconClock, littleBump);

   struct InteractionRamp const * const r = &interactionRamp[state.rampPosition];
   struct TerribleAnimation * const ta = &state.animation;

   // set target jitter to animate towards
   state.jitterTarget = r->maxJitter;
   ta->jitterSlew = r->jitterSlew;

   // update duration
   ta->clockDivisor = r->clockDivisor;

   state.frameLengthTarget = r->frameMultiplier * getAnimationClockPeriod(ta);

   iprintf("Anim frame len will be %d ms / %d = %d ms\n", BEACON_CLOCK_DEFAULT_PERIOD_MS, ta->clockDivisor, state.frameLengthTarget);
}

