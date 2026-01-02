#pragma once
#include <stdint.h>

uint64_t getCycleCount(void);
uint64_t getSystemTimeUs(void);
uint64_t getInstructionsRetiredCount(void);

void enableMachineTimerInterrupt(void);
void disableMachineTimerInterrupt(void);

void enableGlobalMachineInterrupt(void);
void disableGlobalMachineInterrupt(void);

uint32_t getHeartId(void);

// Machine external interrupt disable
// This style should convey that this is something special
#define MEI_DIS                                  \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrc mie, %0"                       \
            :                                    \
            : "r"(1u << 11)                      \
            :                                    \
        );                                       \
    } while (0)

// Machine external interrupt enable
// This style should convey that this is something special
#define MEI_EN                                   \
    do {                                         \
        asm volatile (                           \
            ".option arch, +zicsr\n\t"           \
            "csrs mie, %0"                       \
            :                                    \
            : "r"(1u << 11)                      \
            :                                    \
        );                                       \
    } while (0)
