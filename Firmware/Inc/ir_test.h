/*
 * Functions to be used to test the IR functionality of the board. Do not use
 * during normal operation!
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

enum ir_TestTXEnable {
   IR_TXE_ENABLE,
   IR_TXE_DISABLE
};

void ir_TestInit();
void ir_TestSetEnableTX(enum ir_TestTXEnable en);

