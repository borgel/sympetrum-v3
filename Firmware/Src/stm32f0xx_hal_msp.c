#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_tim.h"

#include "iprintf.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);

//FIXME rm? mv?
static void Error_Handler(void) {
   iprintf("ERROR\n");
}

void HAL_MspInit(void)
{
   __HAL_RCC_SYSCFG_CLK_ENABLE();

   /* System interrupt init*/
   /* SVC_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(SVC_IRQn, 0, 0);
   /* PendSV_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
   /* SysTick_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
   GPIO_InitTypeDef GPIO_InitStruct;
   if(huart->Instance==USART1)
   {
      /* Peripheral clock enable */
      __HAL_RCC_USART1_CLK_ENABLE();

      /**USART1 GPIO Configuration    
        PA9      ------> USART1_TX
        PA10     ------> USART1_RX 
       */
      GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   }
   else if(huart->Instance==USART2)
   {
      /* Peripheral clock enable */
      __HAL_RCC_USART2_CLK_ENABLE();

      /**USART2 GPIO Configuration    
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX 
       */
      GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
   if(huart->Instance==USART1)
   {
      /* Peripheral clock disable */
      __HAL_RCC_USART1_CLK_DISABLE();

      /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
       */
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
   }
   else if(huart->Instance==USART2)
   {
      /* Peripheral clock disable */
      __HAL_RCC_USART2_CLK_DISABLE();

      /**USART2 GPIO Configuration    
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX 
       */
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
   }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

   GPIO_InitTypeDef GPIO_InitStruct;
   if(hi2c->Instance==I2C1)
   {
      /* USER CODE BEGIN I2C1_MspInit 0 */

      /* USER CODE END I2C1_MspInit 0 */

      /**I2C1 GPIO Configuration    
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA 
       */
      GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      /* Peripheral clock enable */
      __HAL_RCC_I2C1_CLK_ENABLE();
      /* Peripheral interrupt init */
      HAL_NVIC_SetPriority(I2C1_IRQn, 3, 0);
      HAL_NVIC_EnableIRQ(I2C1_IRQn);
      /* USER CODE BEGIN I2C1_MspInit 1 */

      /* USER CODE END I2C1_MspInit 1 */
   }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

   if(hi2c->Instance==I2C1)
   {
      /* USER CODE BEGIN I2C1_MspDeInit 0 */

      /* USER CODE END I2C1_MspDeInit 0 */
      /* Peripheral clock disable */
      __HAL_RCC_I2C1_CLK_DISABLE();

      /**I2C1 GPIO Configuration    
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA 
       */
      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

      /* Peripheral interrupt DeInit*/
      HAL_NVIC_DisableIRQ(I2C1_IRQn);

   }
   /* USER CODE BEGIN I2C1_MspDeInit 1 */

   /* USER CODE END I2C1_MspDeInit 1 */

}


//copied in
TIM_HandleTypeDef htim16;

/* TIM16 init function */
void MX_TIM16_Init(void)
{
#if 1
   TIM_OC_InitTypeDef sConfigOC;
   TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

   htim16.Instance = TIM16;
   htim16.Init.Prescaler = 0;
   htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim16.Init.Period = 1264; //857 for 56KHz
   htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   htim16.Init.RepetitionCounter = 0;
   htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
   if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
   {
      Error_Handler();
   }

   if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
   {
      Error_Handler();
   }

   sConfigOC.OCMode = TIM_OCMODE_PWM1;
   sConfigOC.Pulse =  632; //429  for 56KHz
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
   if (HAL_TIM_OC_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
   {
      Error_Handler();
   }

   sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
   sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
   sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
   sBreakDeadTimeConfig.DeadTime = 0;
   sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
   sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
   //sBreakDeadTimeConfig.BreakFilter = 0;      //TODO doesn't exist?
   sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
   if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
   {
      Error_Handler();
   }

   HAL_TIM_MspPostInit(&htim16);
#else
   TIM_ClockConfigTypeDef sClockSourceConfig;
   TIM_MasterConfigTypeDef sMasterConfig;
   TIM_OC_InitTypeDef sConfigOC;
   htim16.Instance = TIM16;
   htim16.Init.Prescaler = 0;
   htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim16.Init.Period = 1264; //857 for 56KHz
   htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   HAL_TIM_Base_Init(&htim16);
   //sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
   //HAL_TIM_ConfigClockSource(&htim16, &sClockSourceConfig);
   HAL_TIM_PWM_Init(&htim16);
   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
   HAL_TIMEx_MasterConfigSynchronization(&htim16, &sMasterConfig);
   sConfigOC.OCMode = TIM_OCMODE_PWM1;
   sConfigOC.Pulse = 632; //429  for 56KHz
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1);
   HAL_TIM_MspPostInit(&htim16);
#endif
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

   if(tim_baseHandle->Instance==TIM16)
   {
      /* USER CODE BEGIN TIM16_MspInit 0 */

      /* USER CODE END TIM16_MspInit 0 */
      /* Peripheral clock enable */
      __HAL_RCC_TIM16_CLK_ENABLE();
      /* USER CODE BEGIN TIM16_MspInit 1 */

      /* USER CODE END TIM16_MspInit 1 */
   }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

   GPIO_InitTypeDef GPIO_InitStruct;
   if(timHandle->Instance==TIM16)
   {
      /* USER CODE BEGIN TIM16_MspPostInit 0 */

      /* USER CODE END TIM16_MspPostInit 0 */

      /**TIM16 GPIO Configuration    
        PB8     ------> TIM16_CH1 
       */
      //FIXME not broken out here, working around it
      /*
      GPIO_InitStruct.Pin = TIM_IR_CARRIER_FREQ_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF1_TIM16;
      HAL_GPIO_Init(TIM_IR_CARRIER_FREQ_GPIO_Port, &GPIO_InitStruct);
      */

      /* USER CODE BEGIN TIM16_MspPostInit 1 */

      /* USER CODE END TIM16_MspPostInit 1 */
   }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

   if(tim_baseHandle->Instance==TIM16)
   {
      /* USER CODE BEGIN TIM16_MspDeInit 0 */

      /* USER CODE END TIM16_MspDeInit 0 */
      /* Peripheral clock disable */
      __HAL_RCC_TIM16_CLK_DISABLE();
   }
   /* USER CODE BEGIN TIM16_MspDeInit 1 */

   /* USER CODE END TIM16_MspDeInit 1 */
} 

