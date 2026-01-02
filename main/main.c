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
#include "asm_utils.h"
#include "spi.h"
#include "machineTimerInterrupt.h"
#include "sysInterrupt.h"

void bubbleSort_int32(int32_t arr[], size_t n);

static void printStringInterruptSafe(const char* str) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            uart_putCharBlocking('\r');
        }
        uart_putCharBlocking(str[i]);
    }
}

static void mtimeHandler(void) {
    printStringInterruptSafe("Plonk!\n");
}

static void readStringFromUart(char** b) {
    const size_t defaultBufSize = 32;
    char* buffer = malloc(defaultBufSize);
    if (buffer == NULL) {
        printf("Malloc failed" "\n");
    }
    size_t curSize = defaultBufSize;
    size_t index = 0;
    while(true) {
        char byte = getchar();
        if (byte == 255) {
            continue;
        }
        if (byte == '\r'){
            printf("\n");
        } else {
            uart_putCharBlocking(byte);
        }
        if ((byte >= '0' && byte <= '9') || byte == ',' || byte == '\r' || byte == '-') {
            if (index == curSize - 1) {
                curSize *= 2;
                char* tmp = realloc(buffer, curSize);
                if (tmp == NULL) {
                    printf("Realloc failed" "\n");
                    free(buffer);
                    return;
                } else {
                    buffer = tmp;
                }
            }
            if (byte == '\r') {
                buffer[index] = 0;
            } else {
                buffer[index] = byte;
            }
            index++;
            if (byte == '\r') {
                break;
            }
        }
    }
    *b = buffer;
}

static size_t parseNumbers(const char* buf, int32_t** out) {
    if (buf[0] == 0) {
        return 0;
    }
    size_t index = 0;
    size_t numCount = 0;
    // First step, figure out how many numbers
    while(true) {
        while(buf[index] == ',' && buf[index] != 0) {
            index++;
        }
        if (buf[index] == 0) {
            break;
        }
        numCount++;
        while(buf[index] != ',' && buf[index] != 0) {
            index++;
        }
    }
    int32_t* numbers = malloc(sizeof(uint32_t)*numCount);
    //Second step: getting the actual numbers out
    index = 0;
    size_t numIndex = 0;
    while(true) {
        while(buf[index] == ',' && buf[index] != 0) {
            index++;
        }
        if (buf[index] == 0) {
            break;
        }
        long num = strtol(&buf[index], NULL, 10);
        if (num >= INT32_MAX) {
            numbers[numIndex] = INT32_MAX;
        } else if (num <= INT32_MIN) {
            numbers[numIndex] = INT32_MIN;
        } else {
            numbers[numIndex] = num;
        }
        numIndex++;
        while(buf[index] != ',' && buf[index] != 0) {
            index++;
        }
    }
    *out = numbers;
    return numCount;
}

static void printNumbers(const int32_t* arr, size_t arrlen) {
    for (size_t i = 0; i < arrlen - 1; ++i) {
        printf("%" PRId32 ", ", arr[i]);
    }
    printf("%" PRId32 "\n", arr[arrlen -1]);
}

int main() {
    gpio_setPinInoutType(GPIO_PIN_SPI_SS, GPIO_INOUT_TYPE_OUT);
    gpio_setPin(GPIO_PIN_SPI_SS, false);
    spi_init(10000000, false, false);
    uint8_t spiDataBuf[2] = {0xff, 0x00};
    spi_transaction(&spiDataBuf[0], 2, NULL, 0);
    gpio_setPin(GPIO_PIN_SPI_SS, true);

    gpio_setPinInoutType(GPIO_PIN_IO0, GPIO_INOUT_TYPE_IN);
    uart_init(115200);

    constexpr uint64_t ONE_MS = 1000;
    constexpr uint64_t ONE_S = 1000*ONE_MS;
    setupMachineTimerInterrupt(5*ONE_S, mtimeHandler);

    MGI_EN;

    char *buf = NULL;
    printf("Hello, world!" "\n");
    while (true) {
        int ret = printf("The current systemtime is %" PRId64 "\n", getSystemTimeUs());
        if (ret == 0) return 1;
        ret = printf("The total amount of instructions retired is: %" PRId64 "\n", getInstructionsRetiredCount());
        if (ret == 0) return 1;
        ret = printf("The total amount of cycles passed is %" PRId64 "\n", getCycleCount());
        if (ret == 0) return 1;
        float ratio = ((float)getCycleCount())/((float)getInstructionsRetiredCount());
        printf("The ratio between cycles and instructions retired is %f\n", ratio);
        if (ret == 0) return 1;
        ret = printf("The system clock speed is %" PRId32 "Hz\n", getClockSpeedHz());
        bool pinState = gpio_isPinHigh(GPIO_PIN_IO0);
        printf("Pin 0 is %s" "\n", pinState ? "high" : "low");

        readStringFromUart(&buf);
        if (buf != NULL) {
            if (buf[0] != 0) {
                int32_t *numbers = NULL;
                size_t numCount = parseNumbers(buf, &numbers);
                printf("There are %zu numbers in this string\n", numCount);
                if (numbers != NULL && numCount > 0) {
                    printf("Your numbers are:\n");
                    printNumbers(numbers, numCount);
                    bubbleSort_int32(numbers, numCount);
                    printf("Your numbers, but sorted are:\n");
                    printNumbers(numbers, numCount);
                }
            } else {
                printf("There were no numbers in this string\n");
            }
            free(buf);
            buf = NULL;
        } else {
            printf("Buf is NULL, this indicates program failure\n");
        }
    }
    return 0;
}
