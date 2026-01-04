#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum exceptionManager_ExceptionSource {
    ExceptionSource_InstructionAddressMisaligned,
    ExceptionSource_InstructionAccessFault,
    ExceptionSource_IllegalInstruction,
    ExceptionSource_LoadAddressMisaligned,
    ExceptionSource_LoadAccessFault,
    ExceptionSource_StoreAddressMisaligned,
    ExceptionSource_StoreAccessFault,
    ExceptionSource_UnknownSource
};

// Setting fPtr to nullptr reinstalls a default handler that just loops forever
void exceptionManager_setFallbackHandler(void (*fPtr)(enum exceptionManager_ExceptionSource));

#ifdef __cplusplus
}
#endif
