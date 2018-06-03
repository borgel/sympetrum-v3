#include "test_patterns.h"

#include "iprintf.h"

#include "led.h"
#include "color.h"
#include "platform_hw.h"

#include <stdint.h>
#include <stdbool.h>

// 45 * 2 elements of a sin table
static uint8_t const HueTable[] = {
         254,
         254,
         254,
         253,
         253,
         252,
         251,
         250,
         249,
         248,
         246,
         245,
         243,
         241,
         239,
         237,
         235,
         232,
         230,
         227,
         224,
         221,
         218,
         215,
         212,
         209,
         205,
         202,
         198,
         194,
         191,
         187,
         183,
         179,
         175,
         170,
         166,
         162,
         158,
         153,
         149,
         145,
         140,
         136,
         131,
         127,
         123,
         118,
         114,
         109,
         105,
         101,
         96,
         92,
         88,
         84,
         79,
         75,
         71,
         67,
         64,
         60,
         56,
         52,
         49,
         45,
         42,
         39,
         36,
         33,
         30,
         27,
         24,
         22,
         19,
         17,
         15,
         13,
         11,
         9,
         8,
         6,
         5,
         4,
         3,
         2,
         1,
         1,
         0,
         0,
         0
};

//FIXME rm
enum TestPattern {
   TP_Start,
   TP_ChasingCycle,
   TP_ChasingVertical,
   TP_Polar,
   TP_End,
};


#define BUTTON_DEBOUNCE_MS    150
static uint32_t lastUserButton = 0;

// just paint the display white
static void fillWhite(void) {
   struct color_ColorHSV c = {.h = 10, .s = 0, .v = 255};
   int row, col;

   iprintf("Fill display white forever\n");
   while(true) {
      for(row = 0; row < 4; row++) {
         for(col = 0; col < 12; col++) {
            iprintf("[%d,%d]\n", row, col);

            //led_ClearDisplay();
            led_DrawPixel(row, col, &c);

            //HAL_Delay(500);
         }
      }
   }
}

void TestPatterns_Start(void) {
   struct color_ColorHSV color = {.h = 10, .s = 255, .v = 255};

   enum TestPattern activePattern = TP_Start + 1;

   int huePhase = 0;
   int count = 0;
   while(true) {
      if(lastUserButton && (HAL_GetTick() - lastUserButton > BUTTON_DEBOUNCE_MS)) {
         lastUserButton = 0;

         iprintf("Button\n");

         // change test pattern
         activePattern++;
         if(activePattern >= TP_End) {
            activePattern = TP_Start + 1;
         }

      }
      //permute
      /*
      if(count % 1000) {
         for(int i = 0; i < ChasingSize; i++) {
            struct coord const * c = &chasingArray[i];
            color.h = huePhase + (i * 10);
            led_DrawPixel(c->x, c->y, &color);
         }
      }
      */
      if(activePattern == TP_ChasingCycle) {
         if(count % 500) {
            for(int i = 0; i < 12 * 4; i++) {
               int v = (((float)huePhase + (float)i * 6.0) / 255.0) * (float)sizeof(HueTable);
               v %= (int)sizeof(HueTable);
               color.h = HueTable[v];

               led_DrawPixelLinear(i, &color);
            }
            huePhase += 1;
         }
      }

      if(activePattern == TP_ChasingVertical) {
         if(count % 500) {
            for(int i = 0; i < 12 * 2; i++) {
               int v = (((float)huePhase + (float)i * 7.0) / 255.0) * (float)sizeof(HueTable);
               v %= (int)sizeof(HueTable);
               color.h = HueTable[v];

               led_DrawPixelLinear(i, &color);
               led_DrawPixelLinear(47 - i, &color);

            }
            huePhase += 1;
         }
      }

      if(activePattern == TP_Polar) {
         if(count % 1000) {
            //MATRIX_POLAR_RINGS = 18
            for(int i = 0; i < 18; i++) {
               //color.h = huePhase + (i * 10);

               //int v = (huePhase + i * 1) % 45;

               // pitch of 2 to ~9 look ok
               int v = (((float)huePhase + (float)i * 6.0) / 255.0) * (float)sizeof(HueTable);
               v %= (int)sizeof(HueTable);
               color.h = HueTable[v];

               led_DrawRing(i, &color);
            }
         }

         if(count % 1000) {
            huePhase += 1;
         }
      }
      count++;
   }
}
void TestPatterns_Button(void) {
   if(lastUserButton == 0) {
      lastUserButton = HAL_GetTick();
   }
}

