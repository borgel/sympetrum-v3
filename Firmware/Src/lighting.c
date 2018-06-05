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

static const enum led_Brightness ALSEffectMap[] = {
   [ALC_Start]             = LED_DIV_4,   // not used
   [ALC_IndoorDark]        = LED_DIV_4,
   [ALC_IndoorLight]       = LED_DIV_3,
   [ALC_OutdoorShade]      = LED_DIV_1,
   [ALC_Sunlight]          = LED_DIV_1,
   [ALC_End]               = LED_DIV_4,   // not used
};

struct State {
   uint32_t                      alsInProgress;
   uint32_t                      lastALS;
   enum als_LightCondition       lastCondition;

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

   // start off assuming low brightness
   state.lastCondition = ALC_IndoorDark;

   als_Init();

   // start a reading to pickup ASAP
   als_StartReading();

   // start the interpolator (we'll leave it running forever). This triggers an init of the LED HW
   yabi_setStarted(true);
}

void lighting_Timeslice(uint32_t const timeMS) {
   // always share time with YABI
   yabi_giveTime(timeMS);

   // make sure the LED infra can start transfers
   led_Timeslice();

   if(timeMS - state.lastALS > ALS_POLL_INTERVAL_MS) {
      if(!state.alsInProgress) {
         state.alsInProgress = true;
         als_StartReading();
         return;
      }

      enum als_LightCondition condition;
      if(als_GetLux(&condition)) {
         // we got a valid reading. Reset things
         state.alsInProgress = false;
         state.lastALS = timeMS;

         if(condition != state.lastCondition) {
            state.lastCondition = condition;

            iprintf("Light condition = %d/%d\n", condition, ALC_End - 1);

            // act on ALS reading
            led_SetGlobalBrightness(ALSEffectMap[condition], 255);
         }
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
 * This is called by YABI whenever it wants to set a value to an output device
 */
static void _yabiSetChannelCB(yabi_ChanID chan, yabi_ChanValue value) {
   // set the H value for the relevant pixel
   // NOTE: V is reset inside LED
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
