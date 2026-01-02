#include <stdint.h>
#include <stddef.h>

#include "uart.h"
#include "sysInterrupt.h"

static constexpr uintptr_t mtime_base = 0x4000;

static volatile uint64_t* const mtime = (volatile uint64_t*)(mtime_base + 0x0);
static uint64_t* const mtimecmp = (uint64_t*)(mtime_base + 0x8);

static uint64_t interval;
static void (*handler)(void);

 __attribute__ ((interrupt ("machine"))) void machineTimerInterruptHandler (void){
     (*handler)();
     *mtimecmp += interval;
}

void setupMachineTimerInterrupt(uint64_t intervalUs, void (*fPtr)(void)) {
    if (intervalUs == 0 || fPtr == nullptr) {
        MTI_DIS;
    } else {
        *mtimecmp = 0;
        *mtime = 0;
        interval = intervalUs;
        handler = fPtr;
        MTI_EN;
    }
}
