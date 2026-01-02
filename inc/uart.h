#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void uart_init(uint32_t baudrate);

uint8_t uart_getCharBlocking(void);

void uart_putCharBlocking(uint8_t);

uint16_t uart_getRxCharsAvailable(void);

void uart_flush(void);

#ifdef __cplusplus
}
#endif
