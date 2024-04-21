#include <errno.h>
#include <stdint.h>

#include "gpio.h"

static const size_t TOTAL_PIN_COUNT = 8;

static volatile uint8_t* const gpioConfig = (volatile uint8_t*)0x3000;
static volatile uint8_t* const gpioData = (volatile uint8_t*)0x3008;

int gpio_setPinInoutType(size_t pinIndex, enum GPIO_INOUT_TYPE inoutType) {
    if (pinIndex > TOTAL_PIN_COUNT) {
        return EINVAL;
    }

    gpioConfig[pinIndex] = inoutType;
    return 0;
}

int gpio_setPin(size_t pinIndex, bool setHigh) {
    if (pinIndex > TOTAL_PIN_COUNT) {
        return EINVAL;
    }

    gpioData[pinIndex] = setHigh ? 1 : 0;
    return 0;
}

int gpio_getPin(size_t pinIndex, bool *isHigh) {
    if (pinIndex > TOTAL_PIN_COUNT) {
        return EINVAL;
    }
    *isHigh = (bool)(gpioData[pinIndex] & 0x01);
    return 0;
}
