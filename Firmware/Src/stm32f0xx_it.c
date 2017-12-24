/*
 * The root of all ISRs for the entire program live in this file.
 */
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_tim_ex.h"

#include "platform_hw.h"
#include "ir_encode.h"
#include "ir_decode.h"
#include "led.h"

#include "main.h"
#include "iprintf.h"

extern I2C_HandleTypeDef hi2c1;

//TODO find a better way to pass these in
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;

//TODO move these out of this file (into RC5?)?
static uint32_t ICValue2 = 0;
static uint8_t pol;

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
   HAL_IncTick();
   HAL_SYSTICK_IRQHandler();
}

// Handle events on pins 0-1
void EXTI0_1_IRQHandler(void) {
}

// Handle things on pins 4-15
void EXTI4_15_IRQHandler(void) {
}

/**
* @brief This function handles I2C1 event global interrupt / I2C1 wake-up interrupt through EXTI line 23.
*/
void I2C1_IRQHandler(void)
{
  if (hi2c1.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR)) {
    HAL_I2C_ER_IRQHandler(&hi2c1);
  } else {
    HAL_I2C_EV_IRQHandler(&hi2c1);
  }
}

/*
 * Handle matrix draw tick
 */
void TIM14_IRQHandler(void) {
   __HAL_TIM_CLEAR_IT(&htim14, TIM_FLAG_UPDATE);

   //FIXME rm?
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

   led_UpdateDisplay();

   //FIXME rm
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/*
 * Handle the bit clock ISR for sending IR.
 */
/*
void TIM16_IRQHandler(void)
{
   //figure out the next stage of the outgoing signal
   ir_SignalGenerate();

   //Clear TIM16 update interrupt
   __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);
}
*/

/*
 * Handle the ISR used when decoding incoming IR.
 */
void TIM3_IRQHandler(void)
{
   /* Clear the TIM2 Update pending bit (but doesn't clear the flag)*/
   __HAL_TIM_CLEAR_IT(&htim3, TIM_FLAG_UPDATE);

   // track the time between ALL edges of the incoming signal.
   if(__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_CC1))
   {
      __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC1);

      ICValue2 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);

      //get current polarity and assume we just saw the opposite edge
      pol = (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6));

      ir_DataSampling(ICValue2, pol);
   }
   //check for IR bit timeout
   else if(__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE))
   {
      /* Clears the IR_TIM's pending flags*/
      __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);

      ir_ResetPacket();
   }
}

