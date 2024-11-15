#include <stdint.h>

#include "staticSocInfo.h"
#include "spi.h"

static const uint16_t MAX_QUEUE_SIZE = 16;

static volatile uint8_t* const configureReg = (volatile uint8_t*)0x1010;

static volatile uint8_t* const txQueue = (volatile uint8_t*)0x1011;
static volatile uint8_t* const rxQueue = (volatile uint8_t*)0x1012;

static volatile uint16_t* const txQueueCount = (volatile uint16_t*)0x1014;
static volatile uint16_t* const rxQueueCount = (volatile uint16_t*)0x1016;

static volatile uint32_t* const clockDivisor = (volatile uint32_t*)0x1018;

static void flush(void) {
    uint8_t tmp = *configureReg;
    // Only set enable to false
    tmp &= 0xfe;
    *configureReg = tmp;
    // Re-enable
    tmp |= 0x01;
    *configureReg = tmp;
}

void spi_init(uint32_t speed_hz, bool cpol, bool cpha) {
    *configureReg = 0;
    *clockDivisor = getClockSpeedHz()/speed_hz;
    uint8_t configureByte = 1;
    if (cpol) {
        configureByte |= 1 << 1;
    }
    if (cpha) {
        configureByte |= 1 << 2;
    }
    *configureReg = configureByte;
}

void spi_transaction(uint8_t* txBuf, size_t txBufLen, uint8_t* rxBuf, size_t rxBufLen) {
    size_t txTransactionLength = txBufLen;
    if (rxBufLen > txTransactionLength) {
        txTransactionLength = rxBufLen;
    }
    size_t rxTransactionLength = txTransactionLength;

    flush();
    while(txTransactionLength > 0 || rxTransactionLength > 0) {
        while(*rxQueueCount > 0 && rxTransactionLength > 0) {
            uint8_t dataIn = *rxQueue;
            if (rxBufLen > 0) {
                *rxBuf = dataIn;
                rxBuf++;
                rxBufLen--;
            }
            rxTransactionLength--;
        }

        while(*txQueueCount < MAX_QUEUE_SIZE && txTransactionLength > 0) {
            if (txBufLen > 0) {
                *txQueue = *txBuf;
                txBuf++;
                txBufLen--;
            } else {
                *txQueue = 0;
            }
            txTransactionLength--;
        }

    }
}
