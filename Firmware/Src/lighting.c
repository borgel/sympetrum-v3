#include "lighting.h"

#include "led.h"
#include "als.h"
#include "platform_hw.h"
#include "matrix_assignments.h"
#include "iprintf.h"

#include "yabi/yabi.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ALS_POLL_INTERVAL_MS        (10 * 1000)

struct State {
   uint32_t alsInProgress;
   uint32_t lastALS;

   //the data which backs YABI's channels. At this level it's basically 48
   //hue buckets that are used internally to track setpoints etc
   struct yabi_ChannelRecord     yabiBacking[TOTAL_LOGICAL_LEDS];
};
static struct State state = {0};

static void setupYABI(void);
static void* const _yabiHwInit(void);
static yabi_ChanValue _rolloverInterpolator(yabi_ChanValue current, yabi_ChanValue start, yabi_ChanValue end, float fraction, float absoluteFraction);
static void _yabiSetChannelCB(yabi_ChanID chan, yabi_ChanValue value);

void lighting_Init(void) {
   setupYABI();

   als_Init();

   // TODO take and handle one ALS sample to set the tone

   // TODO coordinate animations

   // start the interpolator (we'll leave it running forever). This triggers an init of the LED HW
   yabi_setStarted(true);
}

void lighting_Timeslice(uint32_t const timeMS) {
   // always share time with YABI
   yabi_giveTime(timeMS);

   //TODO ALS
   if(timeMS - state.lastALS > ALS_POLL_INTERVAL_MS) {
      if(!state.alsInProgress) {
         state.alsInProgress = true;
         als_StartReading();
         return;
      }

      enum ALS_LightCondition condition;
      if(als_GetLux(&condition)) {
         // we got a valid reading. Reset things
         state.alsInProgress = false;
         state.lastALS = timeMS;

         // FIXME rm
         iprintf("Light condition = %d\n", condition);

         //TODO act on ALS reading
      }
   }
}


void lighting_DrawPixelLinear(uint8_t x, struct color_ColorHSV * const color, uint8_t maxJitter, uint32_t durationMS) {
   yabi_Error res;

   // apply jitter
   if(maxJitter) {
      int16_t jitter = rand() % maxJitter;
      // re-center around 0
      jitter -= jitter / 2;
      // apply jitter
      color->h += jitter;
   }

   res  = yabi_setChannel(x, color->h, durationMS);
   if(res != YABI_OK) {
      iprintf("YABI set returend %d\n", res);
   }

}
void lighting_DrawRing(uint8_t r, struct color_ColorHSV * const color, uint8_t maxJitter, uint32_t durationMS) {
   if(r >= MATRIX_POLAR_RINGS) {
      iprintf("Illegal ring request (%d)\n", r);
      return;
   }

   for(int i = 0; MatrixMapPolar[r][i] != MATRIX_NO_LED; i++) {
      lighting_DrawPixelLinear(MatrixMapPolar[r][i], color, maxJitter, durationMS);
   }
}

// do setup cycle
static void setupYABI(void) {
   yabi_Error yres;

   struct yabi_Config yc = {
      .frameStartCB           = NULL,
      .frameEndCB             = NULL,
      .channelChangeCB        = _yabiSetChannelCB,
      .channelChangeGroupCB   = NULL,
      .interpolator           = _rolloverInterpolator,
      .hwConfig = {
         .setup               = _yabiHwInit,
         .teardown            = NULL,
         .hwConfig            = NULL,
      },
   };

   struct yabi_ChannelStateConfiguration const csc = {
      //FIXME needed?
      .channelStorage = state.yabiBacking,
      .numChannels = TOTAL_LOGICAL_LEDS,
   };

   // setup the channel interpolator
   yres = yabi_init(&yc, &csc);
   if(yres != YABI_OK) {
      iprintf("YABI init returned %d\n", yres);
      return;
   }
}

// called by YABI to init the underlying HW
static void* const _yabiHwInit(void) {
   led_Init();

   memset(state.yabiBacking, 0, sizeof(state.yabiBacking) / sizeof(state.yabiBacking[0]));

   // no context to track
   return NULL;
}

/*
   //FIXME text make sense?
 * This is the hook Yabi calls to set a channel. Yabi doesn't know about HSV, so
 * it uses a mapping scheme to control each parameter. This function is called by
 * Yabi, applies the mapping, then applies the change to the LEDs.
 * The mapping is the obvious one:
 * 0 - H
 * 1 - S
 * 2 - V
 *
 * Example channel math for Yabi setting channel 28
 * 28/3 = 9 (9th LED)
 * 28%3 = 1 (S conponent)
 */
static void _yabiSetChannelCB(yabi_ChanID chan, yabi_ChanValue value) {
   // set the H value for the relevant pixel
   //TODO set S and V to anything in particular? are those reset in led?
   struct color_ColorHSV c = {.h = value, .s = 255, .v = 255};
   led_DrawPixelLinear(chan, &c);
}

// a linear interpolator that knows when to make use of rollover
static yabi_ChanValue _rolloverInterpolator(yabi_ChanValue current, yabi_ChanValue start, yabi_ChanValue end, float fraction, float absoluteFraction) {
   bool increasing;
   uint32_t absoluteTarget = 0;
   uint8_t mod = 0;

   if(end > start)   // XXX increasing
   {
      increasing = true;

      if( end - start > (start + 0xFF) - end) {
         mod = 0xFF;
         increasing = false;
      }
   }
   else     // XXX decreasing
   {
      increasing = false;

      if( start - end > (end + 0xFF) - start) {
         mod = 0xFF;
         increasing = true;
      }
   }

   //FIXME rm
   //iprintf("          old, cur, target %d, %d, %d\n", start, current, end);

   if(increasing) {
      absoluteTarget = start + (uint32_t)(absoluteFraction * (float)((float)(end + mod) - (float)start));
      return (uint8_t)absoluteTarget;
   }
   else {
      absoluteTarget = start - (uint32_t)(absoluteFraction * (float)((float)((float)start + (float)mod) - (float)end));
      return (uint8_t)absoluteTarget;
   }
}
