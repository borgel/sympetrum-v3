/*
 * ir.c
 *
 *  Created on: Jul 12, 2016
 *      Author: alvaro
 *
 *      DCDN IR Library
 *
 *      Theory of operation:
 *      The IR tx/rx library encodes data using pulse widths modulated at 38kHz
 *
 *      A bit is encoded by a 'mark' and a 'space'.
 *
 *      A mark is always the same width during transmission, but the space
 *      changes width depending on whether the bit is a zero (short space)
 *      or one (long space).
 *
 *      A transmission is delimited by a start/stop pulse which consists of
 *      a longer 'mark' followed by a space. Every transmission starts and ends
 *      with the long marks.
 *
 *      During a transmit, a mark consists of modulated 38kHz signal. A space
 *      is the absence of that signal. During a receive, a mark consists of
 *      0V and a space is 3.3V. This is due to the Vishay TSOP receivers which
 *      have active-low outputs.
 *
 *      TIM2 is used to generate the 38kHz signal on IR_TIM2_CH2_Pin
 *      and is connected to IR_UART2_TX_Pin through an IR LED and resistor.
 *      Turning IR_UART2_TX_Pin to 1 enables transmission while 0 disables it.
 *
 *      TIM3 is used as a timer to generate spaces and marks of particular
 *      widths during transmission. During reception, TIM3 is used to measure
 *      the incoming pulse widths.
 *
 */

#include "ir.h"
#include "platform_hw.h"
#include "iprintf.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_gpio.h"
#include <stdlib.h>
#include <stdint.h>

//adapt our macros to darknet lingo
//#define IR_TX_PIN          IR_TX_GPIO_Pin
//#define IR_TX_GPIO_PORT    IR_TX_GPIO_Port
#define IR_RCV_Pin         IR_RX_GPIO_Pin
#define IR_RCV_GPIO_Port   IR_RX_GPIO_Port

// Number of TIM3 ticks for mark/space/start pulses
#define TICK_BASE (400)
#define MARK_TICKS (TICK_BASE)
#define START_TICKS (TICK_BASE * 2)
#define SPACE_ZERO_TICKS (TICK_BASE)
#define SPACE_ONE_TICKS (TICK_BASE * 3)

// Margin to account for time delay in measuring input pulses during receive
#define RX_MARGIN (TICK_BASE/2)

// RX Buffer size
#define IR_RX_BUFF_SIZE (256)

// type for internal CRC calculations
typedef uint8_t crc_t;

// States for state machine
typedef enum {
	IR_RX_IDLE = 0,
	IR_RX_START = 1,
	IR_RX_MARK_START = 2,
	IR_RX_MARK = 3,
	IR_RX_SPACE = 4,
	IR_RX_DONE = 5,
	IR_RX_ERR = -1,
	IR_RX_ERR_TIMEOUT = -2,
	IR_RX_ERR_OVERFLOW = -3,
	IR_RX_ERR_CRC = -4
} IRState_t;

typedef enum {
	IR_RX = 0, IR_TX
} IRMode_t;

static volatile IRState_t IRState;
static volatile IRMode_t IRMode;
static volatile uint8_t irRxBuff[IR_RX_BUFF_SIZE];
static volatile uint32_t irRxBits;
static volatile crc_t crc;
//FIXME what do I replace this with?
//static const IRQn_Type IR_RECV_IRQ = EXTI2_TSC_IRQn;
static const IRQn_Type IR_RECV_IRQ = EXTI2_3_IRQn;
static bool ShouldRX = false;

TIM_HandleTypeDef htim3;
static TIM_HandleTypeDef htim17;

static void TIM17_Init(void);

static crc_t crc_init(void);
static crc_t crc_finalize(crc_t crc);
static crc_t crc_update(crc_t crc, const void *data, size_t data_len);

//
// Configure timer 3 to measure incoming IR pulse widths
// 48MHz clock divided by 32 gives us 1.5MHz timer clock
// With a 4096 period, timer overflows in ~2.73ms
//
void TIM3_Init() {

	TIM_ClockConfigTypeDef sClockSourceConfig;

	__HAL_RCC_TIM3_CLK_ENABLE();

   iprintf("4");

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 32;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 4096;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim3);

   iprintf("5");

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

   iprintf("6");

	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_SR_UIF);

   //FIXME set here? in HAL_MSP?
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

   iprintf("7");
}

/*
 * Generated the 38kHz output signal when PWM is enabled
 */
static void TIM17_Init(void)
{
   TIM_OC_InitTypeDef sConfigOC;
   TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

   htim17.Instance = TIM17;
   htim17.Init.Prescaler = 0;
   htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim17.Init.Period = 1333;
   htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   htim17.Init.RepetitionCounter = 0;
   htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
   if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
   {
      iprintf("Error\r\n");
      return;
   }

   if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
   {
      iprintf("Error\r\n");
      return;
   }

   sConfigOC.OCMode = TIM_OCMODE_PWM1;
   sConfigOC.Pulse = 333;
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
   if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
   {
      iprintf("Error\r\n");
      return;
   }

   sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
   sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
   sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
   sBreakDeadTimeConfig.DeadTime = 0;
   sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
   sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
   sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
   if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
   {
      iprintf("Error\r\n");
      return;
   }
}

// Wait until a specified number of TIM3 clock ticks elapses
void delayTicks(uint32_t ticks) {
	uint32_t oldTicks = TIM3->ARR; // Save value to be restored later

   iprintf("Delay %d ticks\n", ticks);

	IRMode = IR_TX; // Change mode so the TIM3 isr knows what to do

	// Make sure the timer isn't running anymore
	HAL_TIM_Base_Stop_IT(&htim3);

	TIM3->CNT = 0;
	TIM3->ARR = ticks;

	// Clear any pending interrupts and start counting!
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(&htim3);

   iprintf("about to wait for tim\n");

	// Wait here until the timer overflow interrupt occurs
	while (IRMode == IR_TX) {
		__WFI();
	}

	// Restore auto reload register
	TIM3->ARR = oldTicks;
}

// Start TIM3 to measure incoming pulse width
void startIRPulseTimer() {
	TIM3->CNT = 0;
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(&htim3);
}

void stopIRPulseTimer() {
	HAL_TIM_Base_Stop_IT(&htim3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim3) {
		stopIRPulseTimer();
		if (IRMode == IR_RX) {
			// Timed out :(
			IRState = IR_RX_ERR_TIMEOUT;
		} else if (IRMode == IR_TX) {
			IRMode = IR_RX;
		}
	}
}

void IRInit(void) {
	// IR Transmit GPIO configuration
	GPIO_InitTypeDef GPIO_InitStruct;

	//GPIO_InitStruct.Pin = IR_TX_PIN;// | TIM_IR_CARRIER_FREQ_Pin;//IR_UART2_TX_Pin;
	//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	//HAL_GPIO_Init(IR_TX_GPIO_PORT, &GPIO_InitStruct);

   /*
	GPIO_InitTypeDef GPIO_InitStruct;

   GPIO_InitStruct.Pin = IR_TX_GPIO_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
   HAL_GPIO_Init(IR_TX_GPIO_Port, &GPIO_InitStruct);

	// Turn off IR LED by default
	HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_RESET);
   */

   /*
	GPIO_InitStruct.Pin = TIM_IR_CARRIER_FREQ_Pin;	// | TIM_IR_CARRIER_FREQ_Pin;//IR_UART2_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(IR_TX_GPIO_PORT, &GPIO_InitStruct);
	//tranmission happens when tx pin is high and carrier is low
	HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_SET);
	for(int i=0;i<1000;i++) {
		HAL_GPIO_TogglePin(TIM_IR_CARRIER_FREQ_GPIO_Port,TIM_IR_CARRIER_FREQ_Pin);
		HAL_Delay(25);
	}
   */

	// IR Receive GPIO configuration
	//GPIO_InitStruct.Pin = IR_RCV_Pin;
	//GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	//GPIO_InitStruct.Pull = GPIO_NOPULL;
	//HAL_GPIO_Init(IR_RCV_GPIO_Port, &GPIO_InitStruct);

   // test GPIOs
   //A1, 
   GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_1;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF5_TIM17;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

	// Receive interrupt
	HAL_NVIC_SetPriority(IR_RECV_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(IR_RECV_IRQ);
	ShouldRX = false;
	//HAL_NVIC_DisableIRQ(IR_RECV_IRQ);

   iprintf("3");

	// Pulse measuring timer for receive
	TIM3_Init();
   TIM17_Init();
   iprintf("8");
}

void IRStop() {
	stopIRPulseTimer();
   //TODO stop TIM17?
	HAL_TIM_Base_Stop_IT(&htim3);
	ShouldRX = false;
}

// Transmit start pulse
void IRStartStop(void) {
	//HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_SET);
   HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delayTicks(START_TICKS);
	//HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_RESET);
   HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delayTicks(START_TICKS);
}

// Transmit a zero
void IRZero(void) {
	//HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_SET);
   HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delayTicks(MARK_TICKS);
	//HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_RESET);
   HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delayTicks(SPACE_ZERO_TICKS);
}

// Transmit a one
void IROne(void) {
	//HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_SET);
   HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delayTicks(MARK_TICKS);
	//HAL_GPIO_WritePin(IR_TX_GPIO_PORT, IR_TX_PIN, GPIO_PIN_RESET);
   HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delayTicks(SPACE_ONE_TICKS);
}

void IRTxByte(uint8_t byte) {
	for (int8_t bit = 7; bit >= 0; bit--) {
		if ((byte & (0x01 << bit)) == 0x00) {
			IRZero();
		} else {
			IROne();
		}
	}
}

void IRTxBuff(uint8_t *buff, size_t len) {
	crc = crc_init();

   iprintf("TX start/stop\n");

	IRStartStop();

	for (uint8_t byte = 0; byte < len; byte++) {
      //iprintf("byte ");
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
		IRTxByte(buff[byte]);
		crc = crc_update(crc, (unsigned char *) &buff[byte], 1);
	}

	crc = crc_finalize(crc);

	IRTxByte(crc);

   iprintf("TX start/stop\n");

	IRStartStop();
}

// Shift bits into rx buffer
void IRRxBit(uint8_t newBit) {
	uint32_t byte = irRxBits >> 3;
	uint32_t bit = irRxBits & 0x07;

	// Make sure we don't overflow the receive buffer!
	if (byte >= IR_RX_BUFF_SIZE) {
		IRState = IR_RX_ERR_OVERFLOW;
		return;
	}

	if (newBit == 0) {
		irRxBuff[byte] &= ~(1 << (7 - bit));
	} else {
		irRxBuff[byte] |= (1 << (7 - bit));
	}

	// If full byte has been received, calculate CRC for that byte
	if (bit == 0x7) {
		crc = crc_update(crc, (unsigned char *) &irRxBuff[byte], 1);
	}

	irRxBits++;
}

int32_t IRBytesAvailable() {
	int32_t bytes = (irRxBits >> 3);

	if ((IRState == IR_RX_DONE) && (bytes > 0)) {
		// Don't count CRC byte!
		return bytes - 1;
	} else {
		return 0;
	}
}

void IRStartRx() {
	irRxBits = 0;
	IRState = IR_RX_IDLE;
	//__HAL_GPIO_EXTI_CLEAR_IT(IR_RCV_Pin);
	//HAL_NVIC_EnableIRQ(IR_RECV_IRQ);
	ShouldRX = true;
}

void IRStopRX() {
	IRState = IR_RX_IDLE;
	//HAL_NVIC_DisableIRQ(IR_RECV_IRQ);
	ShouldRX = false;
}

// Block until a packet is received OR the timeout expires
int32_t IRRxBlocking(uint32_t timeout_ms) {
	uint32_t timeout = HAL_GetTick() + timeout_ms;

	IRStartRx();

	while ((IRState != IR_RX_DONE) && !(IRState < 0) && (HAL_GetTick() < timeout)) {
		__WFI();
	}

	if (HAL_GetTick() >= timeout) {
		return IR_RX_ERR_TIMEOUT;
	} else if (IRState < 0) {
		return IRState;
	} else {
		return IRBytesAvailable();
	}
}

// For debug purposes
int32_t IRGetState() {
	return IRState;
}

// Return true if a packet has been received
bool IRDataReady() {
	if (IRState == IR_RX_DONE) {
		return true;
	} else {
		return false;
	}
}

// Get pointer to data buffer
uint8_t *IRGetBuff() {
	return (uint8_t *) irRxBuff;
}

// Receive GPIO state machine
void IRStateMachine() {
	uint32_t count = TIM3->CNT; // Save timer value as soon as possible
	uint32_t pinState = HAL_GPIO_ReadPin(IR_RCV_GPIO_Port, IR_RCV_Pin);

	// Stop timer to prevent overflow
	stopIRPulseTimer();

	// Add margin to account for measurement delays (interrupt latency, etc)
	count += RX_MARGIN;

	switch (IRState) {
		// Idle, waiting for a start pulse
		case IR_RX_IDLE: {
			if (pinState == 0) {
				startIRPulseTimer(); // Start counting
				IRState = IR_RX_START;
			}
			break;
		}

			// Waiting for start pulse to finish
		case IR_RX_START: {
			// Start pulse received! Start getting bits
			if ((pinState == 1) && (count > START_TICKS)) {
				irRxBits = 0;
				crc = crc_init();
				IRState = IR_RX_MARK_START;
			} else {
				// Doesn't look like a start pulse, go back to waiting
				IRState = IR_RX_IDLE;
			}
			break;
		}

		case IR_RX_MARK_START: {
			if (pinState == 0) {
				startIRPulseTimer(); // Start timing mark
				IRState = IR_RX_MARK;
			} else {
				IRState = IR_RX_ERR;
			}
			break;
		}

		case IR_RX_MARK: {
			if (pinState == 0) {
				IRState = IR_RX_ERR;
				break;
			}

			if (count > START_TICKS) {
				crc = crc_finalize(crc);
				if (crc == 0) {
					IRState = IR_RX_DONE;
				} else {
					IRState = IR_RX_ERR_CRC;
				}

				//HAL_NVIC_DisableIRQ(IR_RECV_IRQ);
				ShouldRX = false;
			} else if (count > MARK_TICKS) {
				startIRPulseTimer(); // Start timing space
				IRState = IR_RX_SPACE;
			} else {
				IRState = IR_RX_ERR;
			}
			break;
		}

		case IR_RX_SPACE: {
			if (pinState == 0) {
				startIRPulseTimer(); // Start timing next mark
				IRState = IR_RX_MARK;
				if (count > SPACE_ONE_TICKS) {
					IRRxBit(1);
				} else if (count > SPACE_ZERO_TICKS) {
					IRRxBit(0);
				} else {
					// Something bad happened
					IRState = IR_RX_ERR;
				}
			}
			break;
		}

		case IR_RX_DONE: {
			// check CRC if there is one?
			break;
		}

		default: {
			break;
		}
	}

	// Disable interrupts if an error occurred (until user resets it)
	if (IRState < 0) {
		//HAL_NVIC_DisableIRQ(IR_RECV_IRQ);
		ShouldRX = false;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	// Call IR state machine whenever IR_UART2_RX_Pin changes state
	if ((GPIO_Pin & IR_RCV_Pin) && ShouldRX) {
		IRStateMachine();
	}
}


// moved here from external file
/**
 * \file crc.c
 * Functions and types for CRC checks.
 *
 * Generated on Mon Jul 18 07:07:41 2016,
 * by pycrc v0.9, https://pycrc.org
 * using the configuration:
 *    Width         = 8
 *    Poly          = 0x07
 *    Xor_In        = 0x00
 *    ReflectIn     = False
 *    Xor_Out       = 0x00
 *    ReflectOut    = False
 *    Algorithm     = table-driven
*****************************************************************************/
/**
* Static table used for the table_driven implementation.
*****************************************************************************/
static const crc_t crc_table[16] = {
   0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d
};

/**
 * Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
*****************************************************************************/
static crc_t crc_update(crc_t crc, const void *data, size_t data_len)
{
   const unsigned char *d = (const unsigned char *)data;
   unsigned int tbl_idx;

   while (data_len--) {
      tbl_idx = (crc >> 4) ^ (*d >> 4);
      crc = crc_table[tbl_idx & 0x0f] ^ (crc << 4);
      tbl_idx = (crc >> 4) ^ (*d >> 0);
      crc = crc_table[tbl_idx & 0x0f] ^ (crc << 4);

      d++;
   }
   return crc & 0xff;
}


/**
 * Calculate the initial crc value.
 *
 * \return     The initial crc value.
*****************************************************************************/
static crc_t crc_init(void)
{
       return 0x00;
}
/**
 * Calculate the final crc value.
 *
 * \param crc  The current crc value.
 * \return     The final crc value.
*****************************************************************************/
static crc_t crc_finalize(crc_t crc)
{
       return crc ^ 0x00;
}



