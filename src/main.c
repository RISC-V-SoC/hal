#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>

#include "staticSocInfo.h"
#include "gpio.h"
#include "uart.h"
#include "bubbleSort.h"
#include "asm_utils.h"
#include "spi.h"

#define LINE_ENDING "\r\n"
#define BUF_SIZE 9

static void putString(const char* str) {
    while (*str) {
        uart_putCharBlocking(*str);
        str++;
    }
}

int main() {
    uart_init(115200);
    // Configure slave select/chip select pin to be out
    gpio_setPinInoutType(GPIO_PIN_SPI_SS, GPIO_INOUT_TYPE_OUT);
    // Pull the pin high to deselect the matrices
    gpio_setPin(GPIO_PIN_SPI_SS, true);
    // Initialize the spi device to 10 MHz
    spi_init(10000000, false, false);
    uint8_t dataToSend[BUF_SIZE];
    for (size_t i = 0; i < BUF_SIZE; i++) {
        dataToSend[i] = i;
    }
    uint8_t dataReceived[BUF_SIZE];
    gpio_setPin(GPIO_PIN_SPI_SS, false);
    spi_transaction(&dataToSend[0], BUF_SIZE, &dataReceived[0], BUF_SIZE);
    gpio_setPin(GPIO_PIN_SPI_SS, true);
    for (size_t i = 0; i < BUF_SIZE; i++) {
        if (dataReceived[i] != dataToSend[i]) {
            putString("NOK"LINE_ENDING);
            return 1;
        }
    }
    memset(dataReceived, 0, BUF_SIZE);
    gpio_setPin(GPIO_PIN_SPI_SS, false);
    spi_transaction(&dataToSend[0], BUF_SIZE, NULL, 0);
    gpio_setPin(GPIO_PIN_SPI_SS, true);
    for (size_t i = 0; i < BUF_SIZE; i++) {
        if (dataReceived[i] != dataToSend[i]) {
            putString("NOK"LINE_ENDING);
            return 1;
        }
    }
    putString("OK"LINE_ENDING);
    return 0;
}
