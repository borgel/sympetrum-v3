#ifndef PLATFORM_HW_H__
#define PLATFORM_HW_H__

#include <stdbool.h>
#include "stm32f0xx_hal.h"

// for darknet
//#define IR_TX_GPIO_Pin        GPIO_PIN_1
//#define IR_TX_GPIO_Port       GPIOA
//darknet IR rx pin
#define IR_RX_Pin             GPIO_PIN_3
#define IR_RX_GPIO_Port       GPIOA

bool platformHW_Init(void);

#endif//PLATFORM_HW_H__

