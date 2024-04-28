#pragma once

#include <stdbool.h>
#include <stdlib.h>

enum GPIO_INOUT_TYPE {
    GPIO_INOUT_TYPE_IN = 0,
    GPIO_INOUT_TYPE_OUT = 1
};

enum GPIO_PIN_NAME {
    GPIO_PIN_IO0 = 0,
    GPIO_PIN_IO1 = 1,
    GPIO_PIN_IO2 = 2,
    GPIO_PIN_IO3 = 3,
    GPIO_PIN_IO4 = 4,
    GPIO_PIN_IO5 = 5,
    GPIO_PIN_IO6 = 6,
    GPIO_PIN_IO7 = 7,
    GPIO_PIN_SPI_SS = 8
};

void gpio_setPinInoutType(enum GPIO_PIN_NAME pinName, enum GPIO_INOUT_TYPE inoutType);

void gpio_setPin(enum GPIO_PIN_NAME pinName, bool setHigh);

bool gpio_isPinHigh(enum GPIO_PIN_NAME pinName);
