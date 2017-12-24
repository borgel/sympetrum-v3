#ifndef PLATFORM_HW_H__
#define PLATFORM_HW_H__

#include <stdbool.h>
#include "stm32f0xx_hal.h"

#define nCHG_Pin 				GPIO_PIN_1
#define nCHG_GPIO_Port 			GPIOA

#define ESP_nRST_Pin 			GPIO_PIN_4
#define ESP_nRST_GPIO_Port 		GPIOA

#define ACCEL_INT2_Pin 			GPIO_PIN_11
#define ACCEL_INT2_GPIO_Port 	GPIOA

#define ACCEL_INT1_Pin 			GPIO_PIN_12
#define ACCEL_INT1_GPIO_Port 	GPIOA

// for darknet
#define IR_TX_GPIO_Pin        GPIO_PIN_1
#define IR_TX_GPIO_Port       GPIOA
#define IR_RX_GPIO_Pin        GPIO_PIN_3
#define IR_RX_GPIO_Port       GPIOA

bool platformHW_Init(void);

#endif//PLATFORM_HW_H__

