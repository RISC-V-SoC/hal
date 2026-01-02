#include <stdint.h>

#include "uart.h"
#include "spi.h"
#include "gpio.h"
#include "interruptManager.h"
#include "asm_utils.h"

static constexpr uintptr_t SPI_BASE_ADDRESS = 0x1010;

static volatile uint16_t* const interruptOnTxCount = (volatile uint16_t*)(SPI_BASE_ADDRESS + 12);
static volatile uint16_t* const interruptOnRxCount = (volatile uint16_t*)(SPI_BASE_ADDRESS + 14);

static void spiTxInterrupt(void) {
    uart_putCharBlocking(0x01);
    // Disables interrupt from SPI device
    *interruptOnTxCount = 0;
}

static void spiRxInterrupt(void) {
    uart_putCharBlocking(0x03);
    // Disables interrupt from SPI device
    *interruptOnRxCount = 16;
}

int main() {
    uart_init(115200);
    const uint32_t contextPriority = 0;
    interruptManager_setContextPriority(contextPriority);

    interruptManager_setInterruptHandler(InterruptSource_spiTx, spiTxInterrupt);
    interruptManager_setInterruptPriority(InterruptSource_spiTx, contextPriority + 1);

    interruptManager_setInterruptHandler(InterruptSource_spiRx, spiRxInterrupt);
    interruptManager_setInterruptPriority(InterruptSource_spiRx, contextPriority + 1);

    enableGlobalMachineInterrupt();

    *interruptOnTxCount = 16;
    *interruptOnRxCount = 0;

    uart_putCharBlocking(0x00);

    gpio_setPinInoutType(GPIO_PIN_SPI_SS, GPIO_INOUT_TYPE_OUT);
    gpio_setPin(GPIO_PIN_SPI_SS, false);
    // If everything works as expected, the TX interrupt should occur during the init, since
    // there are less than 16 items in the TX buffer and the device becomes enabled
    spi_init(10000000, false, false);
    uart_putCharBlocking(0x02);
    uint8_t spiDataBuf[1] = {0xff};
    // During this transaction we expect the RX interrupt
    spi_transaction(&spiDataBuf[0], 1, NULL, 0);
    uart_putCharBlocking(0x04);
    gpio_setPin(GPIO_PIN_SPI_SS, true);
}
