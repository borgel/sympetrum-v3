#ifndef __IR_DECODE_H
#define __IR_DECODE_H

#include "stm32f0xx.h"

#include <stdbool.h>
typedef struct
{
   __IO uint8_t FieldBit;   /*!< Field bit */
   __IO uint8_t ToggleBit;  /*!< Toggle bit field */
   __IO uint8_t Address;    /*!< Address field */
   __IO uint8_t Command;    /*!< Command field */
} RC5_Frame_TypeDef;

void ir_InitDecode(void);
bool ir_GetDecoded(uint16_t *raw, RC5_Frame_TypeDef *rc5_frame);

//used internally to decode incoming IR data
void ir_ResetPacket(void);
void ir_DataSampling(uint16_t rawPulseLength, uint8_t edge);

void ir_DecodeDisable(void);
void ir_DecodeEnable(void);

#endif /* __IR_DECODE_H */

