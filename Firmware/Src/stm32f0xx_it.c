/*
 * The root of all ISRs for the entire program live in this file.
 */
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_tim_ex.h"

#include "platform_hw.h"
#include "led.h"
#include "test.h"

#include "main.h"
#include "iprintf.h"

extern I2C_HandleTypeDef hi2c1;

//TODO find a better way to pass these in
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;

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
   if(__HAL_GPIO_EXTI_GET_IT(USER_BUTTON_PIN)) {
      __HAL_GPIO_EXTI_CLEAR_IT(USER_BUTTON_PIN);

      // low is pressed
      GPIO_PinState ps = HAL_GPIO_ReadPin(USER_BUTTON_PORT, USER_BUTTON_PIN);
      main_DoButton(ps == GPIO_PIN_RESET);
   }
}

// Handle things on pins 4-15
void EXTI4_15_IRQHandler(void) {
   if(__HAL_GPIO_EXTI_GET_IT(TP_B8_PIN)) {
      __HAL_GPIO_EXTI_CLEAR_IT(TP_B8_PIN);

      // low is pressed
      GPIO_PinState ps = HAL_GPIO_ReadPin(TP_B8_PORT, TP_B8_PIN);

      test_DoTPButton(TP_B8, ps == GPIO_PIN_SET);
   }
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

   //FIXME rm? Just toggled to test matrix draw time
   HAL_GPIO_WritePin(TP_A5_PORT, TP_A5_PIN, GPIO_PIN_SET);

   led_UpdateDisplay();

   //FIXME rm
   HAL_GPIO_WritePin(TP_A5_PORT, TP_A5_PIN, GPIO_PIN_RESET);
}

// Handle events on pins 0-1
void EXTI2_3_IRQHandler(void) {
   if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2)) {
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
   }
   if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_3)) {
      HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
   }
}

/**
 * @brief This function handles TIM3 global interrupt.
 */
void TIM3_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&htim3);
}

