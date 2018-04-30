#include "platform_hw.h"
#include "iprintf.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

#include <string.h>

// the UART used for iprintf
UART_HandleTypeDef huart1;
// UART2 for ESP
UART_HandleTypeDef huart2;

I2C_HandleTypeDef hi2c1;


static void SystemClock_Config(void);
static void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void setKeepAlivePin(bool enable);

/*
 * Setup all the non specific HW in the system.
 */
bool platformHW_Init(void) {
   // Configure the system clock
   SystemClock_Config();

   // Initialize all configured peripherals
   MX_GPIO_Init();

   // we MUST pull this pin high ASAP before the user releases the power button!
   setKeepAlivePin(true);

   MX_USART1_UART_Init();
   MX_USART2_UART_Init();
   MX_I2C1_Init();

   return true;
}

// set the strap that holds the regulator on. If this is disabled, the
// unit powers off IMMEDIATELY.
static void setKeepAlivePin(bool enable) {
   uint8_t e = (enable) ? GPIO_PIN_SET : GPIO_PIN_RESET;
   HAL_GPIO_WritePin(POWER_EN_PORT, POWER_EN_PIN, e);
}

/** System Clock Configuration
 */
void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct;
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_PeriphCLKInitTypeDef PeriphClkInit;

   /**Initializes the CPU, AHB and APB busses clocks 
    */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
   RCC_OscInitStruct.HSICalibrationValue = 16;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
   RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
   RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   /**Initializes the CPU, AHB and APB busses clocks 
    */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
   {
      Error_Handler();
   }

   PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
   PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
   PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
   {
      Error_Handler();
   }

   /**Configure the Systick interrupt time 
    */
   HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

   /**Configure the Systick 
    */
   HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

   /* SysTick_IRQn interrupt configuration */
   HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
   huart1.Instance = USART1;
   huart1.Init.BaudRate = 115200;
   huart1.Init.WordLength = UART_WORDLENGTH_8B;
   huart1.Init.StopBits = UART_STOPBITS_1;
   huart1.Init.Parity = UART_PARITY_NONE;
   huart1.Init.Mode = UART_MODE_TX_RX;
   huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
   huart1.Init.OverSampling = UART_OVERSAMPLING_16;
   huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
   huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
   if (HAL_UART_Init(&huart1) != HAL_OK)
   {
      Error_Handler();
   }
}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{
   huart2.Instance = USART2;
   huart2.Init.BaudRate = 38400;
   huart2.Init.WordLength = UART_WORDLENGTH_8B;
   huart2.Init.StopBits = UART_STOPBITS_1;
   huart2.Init.Parity = UART_PARITY_NONE;
   huart2.Init.Mode = UART_MODE_TX_RX;
   huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
   huart2.Init.OverSampling = UART_OVERSAMPLING_16;
   huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
   huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
   if (HAL_UART_Init(&huart2) != HAL_OK)
   {
      Error_Handler();
   }
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{
   hi2c1.Instance = I2C1;
   hi2c1.Init.Timing = 0x0000020B;
   hi2c1.Init.OwnAddress1 = 0;
   hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
   hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
   hi2c1.Init.OwnAddress2 = 0;
   hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
   hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
   hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
   if (HAL_I2C_Init(&hi2c1) != HAL_OK)
   {
      Error_Handler();
   }

   /**Configure Analogue filter 
    */
   if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
   {
      Error_Handler();
   }
}

static void MX_GPIO_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   /* GPIO Ports Clock Enable */
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();

   // configure LED matrix control lines
   GPIO_InitStruct.Pin = GPIO_PIN_8;
   //FIXME pull low, float high?
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = 0;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   // IR_TX_Pin init with TIM17

   GPIO_InitStruct.Pin = USER_BUTTON_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(USER_BUTTON_PORT, &GPIO_InitStruct);

   // user button EXTIs
   HAL_NVIC_SetPriority(EXTI0_1_IRQn, 2, 0);
   HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

   // power enable strap line
   GPIO_InitStruct.Pin = POWER_EN_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(POWER_EN_PORT, &GPIO_InitStruct);

   // setup diagnostic testpoints
   GPIO_InitStruct.Pin = TP_A5_PIN | TP_A15_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(TP_A15_PORT, &GPIO_InitStruct);

   GPIO_InitStruct.Pin = TP_B8_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(TP_B8_PORT, &GPIO_InitStruct);

   //FIXME needed?
   HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
   HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
   //TODO add error logging
   iprintf("\r\n\r\n");
   iprintf("ERROR!");
   iprintf("\r\n\r\n");
   while(1) { }
   /* USER CODE END Error_Handler */ 
}

