#include <errno.h>
#include <stdint.h>

#include "gpio.h"

static const size_t TOTAL_PIN_COUNT = 9;

static volatile uint8_t* const gpioConfig = (volatile uint8_t*)0x3000;
static volatile uint8_t* const gpioData = gpioConfig + TOTAL_PIN_COUNT;

void gpio_setPinInoutType(enum GPIO_PIN_NAME pinName, enum GPIO_INOUT_TYPE inoutType) {
    gpioConfig[(size_t)pinName] = inoutType;
}

void gpio_setPin(enum GPIO_PIN_NAME pinName, bool setHigh) {
    gpioData[(size_t)pinName] = setHigh ? 1 : 0;
}

bool gpio_isPinHigh(enum GPIO_PIN_NAME pinName) {
    return (bool)(gpioData[(size_t)pinName] & 0x01);
}
