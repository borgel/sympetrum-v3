#ifndef PLATFORM_HW_H__
#define PLATFORM_HW_H__

#include <stdbool.h>
#include "stm32f0xx_hal.h"

#define IR_TX_Pin          (GPIO_PIN_7)
#define IR_TX_Port         (GPIOA)

#define IR_RX_Pin          (GPIO_PIN_3)
#define IR_RX_Port         (GPIOA)

#define USER_BUTTON_PIN    (GPIO_PIN_0)
#define USER_BUTTON_PORT   (GPIOB)

#define ADC_IN_Pin         (GPIO_PIN_2)
#define ADC_IN_Port        (GPIOA)

#define POWER_EN_PIN       (GPIO_PIN_4)
#define POWER_EN_PORT      (GPIOA)

// right hip TP
#define TP_B8_PIN          (GPIO_PIN_8)
#define TP_B8_PORT         (GPIOB)

// below CPU TP
#define TP_A5_PIN          (GPIO_PIN_5)
#define TP_A5_PORT         (GPIOA)

// left hip TP
#define TP_A15_PIN         (GPIO_PIN_15)
#define TP_A15_PORT        (GPIOA)

bool platformHW_Init(void);

#endif//PLATFORM_HW_H__

