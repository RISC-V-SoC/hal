#include <stdint.h>

#include "staticSocInfo.h"
#include "uart.h"

static const uint16_t MAX_QUEUE_SIZE = 16;

static volatile uint8_t* const txQueue = (volatile uint8_t*)0x1000;
static volatile uint8_t* const rxQueue = (volatile uint8_t*)0x1002;

static volatile uint8_t* const txEnable = (volatile uint8_t*)0x1001;
static volatile uint8_t* const rxEnable = (volatile uint8_t*)0x1003;

static volatile uint16_t* const txQueueCount = (volatile uint16_t*)0x1004;
static volatile uint16_t* const rxQueueCount = (volatile uint16_t*)0x1006;

static volatile uint32_t* const baudDivisor = (volatile uint32_t*)0x1008;

void uart_init(uint32_t baudrate) {
    *txEnable = 0;
    *rxEnable = 0;
    *baudDivisor = getClockSpeedHz()/baudrate;
    *txEnable = 1;
    *rxEnable = 1;
}

uint8_t uart_getCharBlocking(void) {
    while(*rxQueueCount == 0);
    return *rxQueue;
}

void uart_putCharBlocking(uint8_t data) {
    while(*txQueueCount >= MAX_QUEUE_SIZE);
    *txQueue = data;
}

uint16_t uart_getRxCharsAvailable(void) {
    return *rxQueueCount;
}

void uart_flush(void) {
    while(*txQueueCount > 0);
}
