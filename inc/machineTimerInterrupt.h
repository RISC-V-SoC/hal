#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// fPtr is called every interval microseconds, under interrupt.
// If fPtr == NULL or intervalUs == 0, the mtimer logic is disabled
void setupMachineTimerInterrupt(uint64_t intervalUs, void (*fPtr)(void));

#ifdef __cplusplus
}
#endif
