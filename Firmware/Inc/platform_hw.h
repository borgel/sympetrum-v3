#ifndef PLATFORM_HW_H__
#define PLATFORM_HW_H__

#include <stdbool.h>
#include "stm32f0xx_hal.h"

#define IR_TX_Pin          (GPIO_PIN_7)
#define IR_TX_Port         (GPIOA)

#define IR_RX_Pin          (GPIO_PIN_3)
#define IR_RX_Port         (GPIOA)

#define USER_BUTTON_PIN    (GPIO_PIN_4)
#define USER_BUTTON_PORT   (GPIOA)

#define ADC_IN_Pin         (GPIO_PIN_2)
#define ADC_IN_Port        (GPIOA)


bool platformHW_Init(void);

#endif//PLATFORM_HW_H__

