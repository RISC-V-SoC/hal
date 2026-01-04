#include <stdint.h>

#include "exceptionManager.h"

enum mcause_code_definitions {
    mcause_instruction_address_misaligned = 0,
    mcause_instruction_access_fault = 1,
    mcause_illegal_instruction = 2,
    mcause_breakpoint = 3,
    mcause_load_address_misaligned = 4,
    mcause_load_access_fault = 5,
    mcause_store_AMO_address_misaligned = 6,
    mcause_store_AMO_access_fault = 7,
    mcause_environment_call_from_U_mode = 8,
    mcause_environment_call_from_S_mode = 9,
    // 10 is reserved
    mcause_environment_call_from_M_mode = 11,
    mcause_instruction_page_fault = 12,
    mcause_load_page_fault = 13,
    // 14 is reserved
    mcause_store_AMO_page_fault = 15,
    // 16 is reserved
    // 17 is reserved
    mcause_software_check = 18,
    mcause_hardware_error = 19,
    // >=20 is either reserved or for custom use
};

static void defaultExceptionHandler(enum exceptionManager_ExceptionSource) {
    while(true);
}

static void (*fallbackHandler)(enum exceptionManager_ExceptionSource) = defaultExceptionHandler;

static uint32_t getExceptionCode(void) {
    uint32_t exceptionCode;
    asm volatile (
        ".option arch, +zicsr\n\t"\
        "csrr %0, mcause"
        : "=r"(exceptionCode)
        :
        :
    );
    return exceptionCode;
}

static enum exceptionManager_ExceptionSource translateExceptionCode(uint32_t code) {
    enum mcause_code_definitions mcause = (enum mcause_code_definitions)code;
    switch(mcause) {
        case mcause_instruction_address_misaligned:
            return ExceptionSource_InstructionAddressMisaligned;
        case mcause_instruction_access_fault:
            return ExceptionSource_InstructionAccessFault;
        case mcause_illegal_instruction:
            return ExceptionSource_IllegalInstruction;
        case mcause_load_address_misaligned:
            return ExceptionSource_LoadAddressMisaligned;
        case mcause_load_access_fault:
            return ExceptionSource_LoadAccessFault;
        case mcause_store_AMO_address_misaligned:
            return ExceptionSource_StoreAddressMisaligned;
        case mcause_store_AMO_access_fault:
            return ExceptionSource_StoreAccessFault;
        default:
            return ExceptionSource_UnknownSource;
    }
}

__attribute__ ((interrupt ("machine"))) void syncTrapHandler (void) {
    enum exceptionManager_ExceptionSource eSource = translateExceptionCode(getExceptionCode());
    fallbackHandler(eSource);
}

void exceptionManager_setFallbackHandler(void (*fPtr)(enum exceptionManager_ExceptionSource)) {
    if (fPtr == nullptr) {
        fallbackHandler = defaultExceptionHandler;
    } else {
        fallbackHandler = fPtr;
    }
}
