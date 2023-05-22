#include "MFRC522.h"

esp_err_t MFRC522_SendCmd (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t cmd)
{
    esp_err_t retVal;
    uint8_t txData[2] = {registerAddress, cmd};

    spi_transaction_t currTrans = {
        .length = 16,
        .tx_buffer = txData,
        .flags = SPI_TRANS_USE_TXDATA,
    };

    retVal = spi_device_transmit(*spiHandle, &currTrans);

    return retVal;
}