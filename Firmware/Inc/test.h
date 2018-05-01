#pragma once

#include <stdbool.h>

enum TestPoints {
   TP_A5,
   TP_A15,
   TP_B8
};

bool test_EnterTestMode(void);
void test_DoTests(void);

void test_DoTPButton(enum TestPoints tp, bool const buttonPressed);
void test_UserButton(bool const buttonPressed);

