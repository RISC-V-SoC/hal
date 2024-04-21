#pragma once

#include <stdbool.h>
#include <stdlib.h>

enum GPIO_INOUT_TYPE {
    GPIO_INOUT_TYPE_IN = 0,
    GPIO_INOUT_TYPE_OUT = 1
};

int gpio_setPinInoutType(size_t pinIndex, enum GPIO_INOUT_TYPE inoutType);

int gpio_setPin(size_t pinIndex, bool setHigh);

int gpio_getPin(size_t pinIndex, bool *isHigh);
