#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <errno.h>

struct SpiDevice_transfer {
    // If tx_buf is not nullptr, it must contain at least len bytes.
    // tx_buf can be destroyed immidiately after the transaction is started
    uint8_t* tx_buf;
    // If rx_buf is not nullptr, it must be capable of holding at least len bytes
    // If rx_buf is not nullptr, it must be kept alive until trueOnComplete is set to true
    // if rx_buf is not nullptr and trueOnComplete is nullptr, spiDevice_transaction will return EINVAL
    uint8_t* rx_buf;

    size_t len;

    // If cs_change is true, device will be deselected before next transfer
    bool cs_change;

    // If trueOnComplete is not a nullptr it should point to a boolean initialy set to false.
    // When the transaction is completed, this boolean is set to true.
    // This is the way to wait until the rx_buf can be read.
    bool *trueOnComplete;
};

void spiDevice_init(uint32_t speed_hz, bool cpol, bool cpha);

// Even in async cases, this function might still block because of resource availability.
// If there is not enough memory to be able to copy tx_buf, this function returns ENOMEM.
// If the spi_transfer struct is nullptr or otherwise illegal, this function returns EINVAL.
// If everything is ok, this function returns 0.
int spiDevice_transaction(struct SpiDevice_transfer* msg);

#ifdef __cplusplus
}
#endif
