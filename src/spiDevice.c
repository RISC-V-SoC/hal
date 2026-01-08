#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "staticSocInfo.h"
#include "spiDevice.h"
#include "interruptManager.h"
#include "gpio.h"

static constexpr uint16_t MAX_QUEUE_SIZE = 16;
static constexpr uint16_t INTERRUPT_ON_TX_DISABLE_VALUE = 0;
static constexpr uint16_t INTERRUPT_ON_RX_DISABLE_VALUE = MAX_QUEUE_SIZE;
static constexpr uintptr_t SPI_BASE_ADDRESS = 0x1010;
static constexpr size_t LOCAL_BUF_DEPTH = 32;

static volatile uint8_t* const configureReg = (volatile uint8_t*)(SPI_BASE_ADDRESS + 0);

static volatile uint8_t* const txQueue = (volatile uint8_t*)(SPI_BASE_ADDRESS + 1);
static volatile uint8_t* const rxQueue = (volatile uint8_t*)(SPI_BASE_ADDRESS + 2);

static volatile uint16_t* const txQueueCount = (volatile uint16_t*)(SPI_BASE_ADDRESS + 4);
static volatile uint16_t* const rxQueueCount = (volatile uint16_t*)(SPI_BASE_ADDRESS + 6);

static volatile uint32_t* const clockDivisor = (volatile uint32_t*)(SPI_BASE_ADDRESS + 8);

static volatile uint16_t* const interruptOnTxCount = (volatile uint16_t*)(SPI_BASE_ADDRESS + 12);
static volatile uint16_t* const interruptOnRxCount = (volatile uint16_t*)(SPI_BASE_ADDRESS + 14);

static struct {
    volatile bool operationPending[LOCAL_BUF_DEPTH];
    bool cleanupPending[LOCAL_BUF_DEPTH];
    struct SpiDevice_transfer buf[LOCAL_BUF_DEPTH];
    size_t pushPointer;
    size_t popPointer;
} circularBuffer;

static struct SpiDevice_transfer* pushTransferStart(void) {
    while(circularBuffer.operationPending[circularBuffer.pushPointer]);
    return &circularBuffer.buf[circularBuffer.pushPointer];
}

static void pushTransferComplete(void) {
    circularBuffer.operationPending[circularBuffer.pushPointer] = true;
    circularBuffer.pushPointer = (circularBuffer.pushPointer + 1) % LOCAL_BUF_DEPTH;
}

static struct SpiDevice_transfer* tryPopTransfer(void) {
    if (!circularBuffer.operationPending[circularBuffer.popPointer]) {
        return nullptr;
    }
    return &circularBuffer.buf[circularBuffer.popPointer];
}

static void popTransferComplete(void) {
    circularBuffer.cleanupPending[circularBuffer.popPointer] = true;
    circularBuffer.operationPending[circularBuffer.popPointer] = true;
    circularBuffer.popPointer = (circularBuffer.popPointer + 1) % LOCAL_BUF_DEPTH;
}

static void collectGarbage(void) {
    for (size_t i = 0; i < LOCAL_BUF_DEPTH; ++i) {
        if (circularBuffer.cleanupPending[i]) {
            if (circularBuffer.buf[i].tx_buf != nullptr) {
                free(circularBuffer.buf[i].tx_buf);
                circularBuffer.buf[i].tx_buf = nullptr;
            }
            circularBuffer.cleanupPending[i] = false;
        }
    }
}

static struct {
    bool hasTransaction;
    size_t txBytesLeft;
    size_t rxBytesLeft;
    uint8_t* txPtr;
    uint8_t* rxPtr;
    bool cs_change;
    bool* trueOnComplete;
} interruptData;

static size_t min(size_t a, size_t b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

static void triggerInterrupt(void) {
    *interruptOnTxCount = MAX_QUEUE_SIZE + 1;
}

static void spiInterrupt(void) {
    if (!interruptData.hasTransaction) {
        struct SpiDevice_transfer* t = tryPopTransfer();
        if (t == nullptr) {
            *interruptOnTxCount = INTERRUPT_ON_TX_DISABLE_VALUE;
            *interruptOnRxCount = INTERRUPT_ON_RX_DISABLE_VALUE;
            return;
        }
        interruptData.hasTransaction = true;
        interruptData.txBytesLeft = t->len;
        interruptData.rxBytesLeft = t->len;
        interruptData.txPtr = t->tx_buf;
        interruptData.rxPtr = t->rx_buf;
        interruptData.cs_change = t->cs_change;
        interruptData.trueOnComplete = t->trueOnComplete;
        gpio_setPin(GPIO_PIN_SPI_SS, false);
    }

    // First, pull everything that is available
    while (interruptData.rxBytesLeft > 0 && *rxQueueCount > 0) {
        uint8_t byte = *rxQueue;
        if (interruptData.rxPtr != nullptr) {
            *interruptData.rxPtr = byte;
            interruptData.rxPtr++;
        }
        interruptData.rxBytesLeft--;
    }

    // Then push as much as we can
    while (interruptData.txBytesLeft > 0 && *txQueueCount < MAX_QUEUE_SIZE) {
        uint8_t byte = 0;
        if (interruptData.txPtr != nullptr) {
            byte = *interruptData.txPtr;
            interruptData.txPtr++;
        }
        interruptData.txBytesLeft--;
        *txQueue = byte;
    }

    if (interruptData.txBytesLeft > 0) {
        // It does not really matter what we choose here,
        // 1 just makes sure that we fill the queue before its completely empty,
        // preventing interruptions in the SPI data flow
        *interruptOnTxCount = 1;
        *interruptOnRxCount = INTERRUPT_ON_RX_DISABLE_VALUE;
    } else if (interruptData.rxBytesLeft > 0) {
        *interruptOnTxCount = INTERRUPT_ON_TX_DISABLE_VALUE;
        *interruptOnRxCount = min(MAX_QUEUE_SIZE - 1, interruptData.rxBytesLeft - 1);
    } else {
        // Transaction done!
        if (interruptData.cs_change) {
            gpio_setPin(GPIO_PIN_SPI_SS, true);
        }
        interruptData.hasTransaction = false;
        *interruptData.trueOnComplete = true;
        popTransferComplete();
        // Retrigger this interrupt to check if another transaction is waiting
        triggerInterrupt();
    }
}

void spiDevice_init(uint32_t speed_hz, bool cpol, bool cpha) {
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

    *interruptOnTxCount = INTERRUPT_ON_TX_DISABLE_VALUE;
    *interruptOnRxCount = INTERRUPT_ON_RX_DISABLE_VALUE;

    gpio_setPinInoutType(GPIO_PIN_SPI_SS, GPIO_INOUT_TYPE_OUT);
    gpio_setPin(GPIO_PIN_SPI_SS, true);

    interruptManager_setInterruptHandler(InterruptSource_spiTx, spiInterrupt);
    interruptManager_setInterruptPriority(InterruptSource_spiTx, 1);

    interruptManager_setInterruptHandler(InterruptSource_spiRx, spiInterrupt);
    interruptManager_setInterruptPriority(InterruptSource_spiRx, 1);

    for (size_t i = 0; i < LOCAL_BUF_DEPTH; ++i) {
        circularBuffer.operationPending[i] = false;
        circularBuffer.cleanupPending[i] = false;
    }
    circularBuffer.pushPointer = 0;
    circularBuffer.popPointer = 0;
    interruptData.hasTransaction = false;
}

int spiDevice_transaction(struct SpiDevice_transfer* msg) {
    collectGarbage();
    if (msg == nullptr) {
        return EINVAL;
    }

    if (msg->rx_buf != nullptr && msg->trueOnComplete == nullptr) {
        return EINVAL;
    }

    typeof(msg->tx_buf) tx_buf_cpy = nullptr;
    if (msg->tx_buf != nullptr) {
        tx_buf_cpy = malloc(msg->len * sizeof(tx_buf_cpy[0]));
        if (tx_buf_cpy == nullptr) {
            return ENOMEM;
        }
        memcpy(tx_buf_cpy, msg->tx_buf, msg->len * sizeof(tx_buf_cpy[0]));
    }

    struct SpiDevice_transfer* bufItem = pushTransferStart();
    *bufItem = *msg;
    bufItem->tx_buf = tx_buf_cpy;
    pushTransferComplete();

    triggerInterrupt();

    return 0;
}
