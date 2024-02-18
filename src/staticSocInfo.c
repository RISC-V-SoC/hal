#include "staticSocInfo.h"

uint32_t getClockSpeedHz(void) {
    const uint32_t* const baseAddr = (const uint32_t* const)0x100c;
    return *baseAddr;
}
