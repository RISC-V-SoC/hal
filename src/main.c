#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "uart.h"
#include "bubbleSort.h"

static void writeString(char* buf) {
    while(*buf != 0) {
        uart_putCharBlocking(*buf);
        buf++;
    }
}

static void writeEndl(void) {
    uart_putCharBlocking('\r');
    uart_putCharBlocking('\n');
}

static void writeStringAndEndl(char* buf) {
    writeString(buf);
    writeEndl();
}

static void readStringFromUart(char** b) {
    const size_t defaultBufSize = 32;
    char* buffer = malloc(defaultBufSize);
    if (buffer == NULL) {
        writeStringAndEndl("Malloc failed");
    }
    size_t curSize = defaultBufSize;
    size_t index = 0;
    while(true) {
        unsigned char byte = uart_getCharBlocking();
        if (byte == '\r'){
            writeEndl();
        } else {
            uart_putCharBlocking(byte);
        }
        if ((byte >= '0' && byte <= '9') || byte == ',' || byte == '\r' || byte == '-') {
            if (index == curSize - 1) {
                curSize *= 2;
                char* tmp = realloc(buffer, curSize);
                if (tmp == NULL) {
                    writeStringAndEndl("Realloc failed");
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
    char* writeBuf = NULL;
    for (size_t i = 0; i < arrlen - 1; ++i) {
        asprintf(&writeBuf, "%" PRId32 ", ", arr[i]);
        if (writeBuf != NULL) {
            writeString(writeBuf);
            free(writeBuf);
            writeBuf = NULL;
        }
    }
    asprintf(&writeBuf, "%" PRId32, arr[arrlen - 1]);
    writeStringAndEndl(writeBuf);
    free(writeBuf);
    writeBuf = NULL;
}

int main() {
    uart_init(115200);
    char *buf = NULL;
    writeStringAndEndl("Hello, world!");
    while (true) {
        readStringFromUart(&buf);
        if (buf != NULL) {
            if (buf[0] != 0) {
                int32_t *numbers = NULL;
                size_t numCount = parseNumbers(buf, &numbers);
                char* writeBuf = NULL;
                asprintf(&writeBuf, "There are %zu numbers in this string", numCount);
                if (writeBuf != NULL) {
                    writeStringAndEndl(writeBuf);
                    free(writeBuf);
                    writeBuf = NULL;
                }
                if (numbers != NULL && numCount > 0) {
                    writeStringAndEndl("Your numbers are:");
                    printNumbers(numbers, numCount);
                    bubbleSort_int32(numbers, numCount);
                    writeStringAndEndl("Your numbers, but sorted are:");
                    printNumbers(numbers, numCount);
                }
            } else {
                writeStringAndEndl("There were no numbers in this string");
            }
            free(buf);
            buf = NULL;
        } else {
            writeStringAndEndl("Buf is NULL, this indicates program failure");
        }
    }
    return 0;
}
