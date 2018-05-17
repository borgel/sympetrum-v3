#include "als.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_adc.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"

#include "platform_hw.h"
#include "iprintf.h"

// TODO adjust these buckets
static uint32_t const buckets[] = {
   [ALC_Start] = 0,
   // 0 to next is next bucket
   [ALC_IndoorDark] =   50,
   [ALC_IndoorLight] =  2000,
   [ALC_OutdoorShade] = 3700,
   // implicitly, everything higher is sunlight (saturation)
   [ALC_Sunlight] =     10000,
   [ALC_End] = 0,
};

static ADC_HandleTypeDef adcALS;

static bool conversionInProgress;

static bool _ADCConfig(void);

bool als_Init(void) {
   GPIO_InitTypeDef GPIO_InitStruct;

   //setup GPIO
   GPIO_InitStruct.Pin = ADC_IN_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(ADC_IN_Port, &GPIO_InitStruct);

   //TODO start this in the background and pick it up later

   // low priority ADC interrupt
   //HAL_NVIC_SetPriority(ADC1_COMP_IRQn, 2, 0);
   //HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);

   conversionInProgress = false;

   return _ADCConfig();
}

void als_StartReading(void) {
   if(!conversionInProgress) {
      //save power by only enabling the ADC when we need it
      HAL_ADC_Start(&adcALS);
      conversionInProgress = true;
   }
}

// returns true when new data arrives
bool als_GetLux(enum ALS_LightCondition * cond) {
   bool res = false;

   if(conversionInProgress) {
      if (HAL_ADC_PollForConversion(&adcALS, 1000000) == HAL_OK)
      {
         conversionInProgress = false;
         res = true;

         uint32_t rawRead;
         rawRead  = HAL_ADC_GetValue(&adcALS);

         // convert into intensity buckets
         // search downwards until we find the first bucket we are inside
         *cond = ALC_Start + 1;
         for(int i = ALC_End - 1; i > ALC_Start; i--) {
            if(rawRead < buckets[i]) {
               continue;
            }

            // the previous one was it
            *cond = (enum ALS_LightCondition)i;
            return res;
         }
      }

      HAL_ADC_Stop(&adcALS);
   }

   return res;
}

static bool _ADCConfig(void) {
   ADC_ChannelConfTypeDef adcChannel;

   __ADC1_CLK_ENABLE();

   adcALS.Instance = ADC1;

   adcALS.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
   adcALS.Init.Resolution = ADC_RESOLUTION_12B;
   adcALS.Init.DataAlign = ADC_DATAALIGN_RIGHT;
   adcALS.Init.ScanConvMode = DISABLE;
   adcALS.Init.EOCSelection = DISABLE;
   adcALS.Init.ContinuousConvMode = DISABLE;
   adcALS.Init.DiscontinuousConvMode = DISABLE;
   adcALS.Init.ExternalTrigConv = ADC_SOFTWARE_START;
   adcALS.Init.DMAContinuousRequests = ENABLE;

   //sample time even for channel
   //FIXME number of cycles randomly chosen
   adcALS.Init.SamplingTimeCommon = ADC_SAMPLETIME_71CYCLES_5;

   HAL_ADC_Init(&adcALS);

   adcChannel.Channel = ADC_CHANNEL_2;
   adcChannel.Rank = 1;

   if (HAL_ADC_ConfigChannel(&adcALS, &adcChannel) != HAL_OK) {
      iprintf("als: Failed to configure ADC channel\n");
      return false;
   }

   return true;
}

