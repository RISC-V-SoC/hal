#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum interruptManager_InterruptSource {
    InterruptSource_spiTx,
    InterruptSource_spiRx
};

void interruptManager_setContextPriority(uint32_t newPriority);

uint32_t interruptManager_getContextPriority(void);

// Setting the source to nullptr disables the interrupt handling
void interruptManager_setInterruptHandler(enum interruptManager_InterruptSource source, void (*fPtr)(void));

void interruptManager_setInterruptPriority(enum interruptManager_InterruptSource source, uint32_t newPriority);

uint32_t interruptManager_getInterruptPriority(enum interruptManager_InterruptSource source);

#ifdef __cplusplus
}
#endif
