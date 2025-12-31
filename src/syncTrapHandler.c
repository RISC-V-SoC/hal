#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

#ifdef _TINY_BUILD_

void syncTrapHandler(uint32_t, uint32_t) {
}

#else

static const char* exceptionCodeAsString[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Reserved",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Reserved",
    "Store/AMO page fault"
};

static void printString(const char* str) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            uart_putCharBlocking('\r');
        }
        uart_putCharBlocking(str[i]);
    }
}

void syncTrapHandler(uint32_t exceptionCode, uint32_t programCounter) {
    uart_init(115200);
    char buffer[100];
    snprintf(buffer, 100, "!! Irrecoverable exception occured !!\n");
    printString(buffer);
    snprintf(buffer, 100, "Program counter: 0x%08" PRIx32 "\n", programCounter);
    printString(buffer);
    snprintf(buffer, 100, "Exception code: 0x%08" PRIx32 "\n", exceptionCode);
    printString(buffer);
    if (exceptionCode < sizeof(exceptionCodeAsString)/sizeof(exceptionCodeAsString[0])) {
        snprintf(buffer, 100, "Exception reason: %s\n", exceptionCodeAsString[exceptionCode]);
    } else {
        snprintf(buffer, 100, "Exception reason: Unknown\n");
    }
    printString(buffer);
    uart_flush();
}
#endif //_TINY_BUILD_
