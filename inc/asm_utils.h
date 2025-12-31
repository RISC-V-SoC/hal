#pragma once
#include <stdint.h>

uint64_t getCycleCount(void);
uint64_t getSystemTimeUs(void);
uint64_t getInstructionsRetiredCount(void);

void enableMachineTimerInterrupt(void);
void disableMachineTimerInterrupt(void);
void enableMachineInterrupt(void);
