#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint64_t getCycleCount(void);
uint64_t getSystemTimeUs(void);
uint64_t getInstructionsRetiredCount(void);

#ifdef __cplusplus
}
#endif
