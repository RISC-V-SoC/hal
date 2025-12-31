#include <stdint.h>
#include <stddef.h>

#include "uart.h"
#include "asm_utils.h"

static volatile uint64_t* const mtime = (volatile uint64_t*)0x4000;
static uint64_t* const mtimecmp = (uint64_t*)0x4008;

constexpr uint64_t ONE_MS = 1000;
constexpr uint64_t ONE_S = 1000*ONE_MS;

constexpr uint64_t interval = 5*ONE_S;

static void printString(const char* str) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            uart_putCharBlocking('\r');
        }
        uart_putCharBlocking(str[i]);
    }
}

 __attribute__ ((interrupt ("machine"))) void machineTimerInterruptHandler (void){
    printString("Plonk!\n");
    *mtimecmp += interval;
}

void setupMachineTimerInterrupt(void) {
    *mtimecmp = 0;
    *mtime = 0;
    enableMachineTimerInterrupt();
    enableMachineInterrupt();
}
