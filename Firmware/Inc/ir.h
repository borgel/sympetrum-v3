#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void IRInit(void);
//TODO refactor into a de-init?
void IRStop();
void IRTxBuff(uint8_t *buff, size_t len);
int32_t IRRxBlocking(uint32_t timeout_ms);

int32_t IRBytesAvailable();
uint8_t *IRGetBuff(uint32_t *len);
bool IRDataReady();

