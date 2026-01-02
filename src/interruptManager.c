#include "interruptManager.h"

#include <stdint.h>
#include <stddef.h>

#include "asm_utils.h"

static constexpr uintptr_t PLIC_BASE_ADDRESS = 0x10000000;
static constexpr uintptr_t PLIC_INTERRUPT_PRIORITY_BASE_ADDRESS = PLIC_BASE_ADDRESS;
static constexpr uintptr_t PLIC_ENABLE_BITS_BASE_ADDRESS = PLIC_BASE_ADDRESS + 0x2000;
static constexpr uintptr_t PLIC_CONTEXT_PRIORITY_BASE_ADDRESS = PLIC_BASE_ADDRESS + 0x200000;
static constexpr uintptr_t PLIC_CLAIM_COMPLETE_BASE_ADDRESS = PLIC_BASE_ADDRESS + 0x200004;

static void (*handlers[3])(void) = {
    nullptr, // RESERVED
    nullptr, // spi tx handler
    nullptr, // spi rx handler
};

static uint32_t translateToInterruptId(enum interruptManager_InterruptSource source) {
    switch(source)
    {
        case InterruptSource_spiTx:
            return 1;
        case InterruptSource_spiRx:
            return 2;
        default:
            return 0;
    }
}

__attribute__ ((interrupt ("machine"))) void machineExternalInterruptHandler (void) {
    const uint32_t heartId = getHeartId();
    volatile uint32_t* claimComplete = (volatile uint32_t*)(PLIC_CLAIM_COMPLETE_BASE_ADDRESS + heartId * 0x1000);
    uint32_t interruptId = *claimComplete;
    if (interruptId == 0) {
        // Interrupt id 0 is reserved to mean "no interrupt pending"
        return;
    }
    handlers[interruptId]();
    *claimComplete = interruptId;
}

void interruptManager_setContextPriority(uint32_t newPriority) {
    const uint32_t heartId = getHeartId();
    volatile uint32_t* contextPriority = (volatile uint32_t*)(PLIC_CONTEXT_PRIORITY_BASE_ADDRESS + heartId * 0x1000);
    *contextPriority = newPriority;
}

uint32_t interruptManager_getContextPriority(void) {
    const uint32_t heartId = getHeartId();
    volatile uint32_t* contextPriority = (volatile uint32_t*)(PLIC_CONTEXT_PRIORITY_BASE_ADDRESS + heartId * 0x1000);
    return *contextPriority;
}

void interruptManager_setInterruptHandler(enum interruptManager_InterruptSource source, void (*fPtr)(void)) {
    uint32_t interruptId = translateToInterruptId(source);
    if (interruptId == 0) {
        return;
    }
    // Disable interrupts so that an interrupt cannot occur during config
    MEI_DIS;

    handlers[interruptId] = fPtr;

    const uint32_t heartId = getHeartId();
    volatile uint32_t* enableBits = (volatile uint32_t*)(PLIC_ENABLE_BITS_BASE_ADDRESS + heartId*0x80);
    const size_t offset = interruptId/32;
    if (fPtr == nullptr) {
        enableBits[offset] &= ~(1 << interruptId % 32);
    } else {
        enableBits[offset] |= 1 << interruptId % 32;
    }

    MEI_EN;
}

void interruptManager_setInterruptPriority(enum interruptManager_InterruptSource source, uint32_t newPriority) {
    uint32_t interruptId = translateToInterruptId(source);
    if (interruptId == 0) {
        return;
    }
    volatile uint32_t* interruptPriority = (volatile uint32_t*)(PLIC_INTERRUPT_PRIORITY_BASE_ADDRESS);
    interruptPriority[interruptId] = newPriority;
}

uint32_t interruptManager_getInterruptPriority(enum interruptManager_InterruptSource source) {
    uint32_t interruptId = translateToInterruptId(source);
    if (interruptId == 0) {
        return 0;
    }
    volatile uint32_t* interruptPriority = (volatile uint32_t*)(PLIC_INTERRUPT_PRIORITY_BASE_ADDRESS);
    return interruptPriority[interruptId];
}
