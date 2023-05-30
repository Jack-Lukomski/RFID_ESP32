#include "MFRC522.h"

esp_err_t MFRC522_SendCmd (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t cmd)
{
    esp_err_t retVal;
    uint8_t txData[2] = {registerAddress << 1, cmd};

    spi_transaction_t currTrans = {
        .length = 16,
        .tx_buffer = txData,
        .flags = SPI_TRANS_USE_TXDATA,
    };

    retVal = spi_device_transmit(*spiHandle, &currTrans);

    return retVal;
}

esp_err_t MFRC522_WriteRegister (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t value)
{
    esp_err_t retVal;
    uint8_t txData[2] = {(registerAddress << 1) & 0x7E, value}; // The LSB is 0 for write operations

    spi_transaction_t currTrans = {
        .length = 16,
        .tx_buffer = txData,
        .flags = SPI_TRANS_USE_TXDATA,
    };

    retVal = spi_device_transmit(*spiHandle, &currTrans);

    return retVal;
}


esp_err_t MFRC522_ReadRegister(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t *data)
{
    esp_err_t retVal;
    uint8_t txData[2] = {(0x80 | registerAddress) << 1, 0x00};
    uint8_t rxData[2];

    spi_transaction_t currTrans = {
        .length = 16,
        .tx_buffer = txData,
        .rx_buffer = rxData,
        .flags = 0,
    };

    retVal = spi_device_transmit(*spiHandle, &currTrans);

    if (retVal == ESP_OK)
    {
        *data = rxData[0];
    }

    return retVal;
}

esp_err_t MFRC522_Init(spi_device_handle_t *spiHandle)
{
    esp_err_t retVal;

    // Perform soft reset
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_CMDReg_ADDRESS, 0b1111);
    if (retVal != ESP_OK)
    {
        return retVal;
    }
    // Give some time for the module to reset
    vTaskDelay(pdMS_TO_TICKS(50));

    // Set timer: TPrescaler=TModeReg[3:0]:TPrescalerReg, TReload=TReloadReg
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_TModeReg_ADDRESS, 0x80);
    if (retVal != ESP_OK)
    {
        return retVal;
    }
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_TPrescalerReg_ADDRESS, 0xA9);
    if (retVal != ESP_OK)
    {
        return retVal;
    }
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_TReloadRegH_ADDRESS, 0x03);
    if (retVal != ESP_OK)
    {
        return retVal;
    }
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_TReloadRegL_ADDRESS, 0xE8);
    if (retVal != ESP_OK)
    {
        return retVal;
    }
    // Force100ASK
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_TxASKReg_ADDRESS, 0x40);
    if (retVal != ESP_OK)
    {
        return retVal;
    }

    // Set CRC to 0x6363 (ISO 14443-3 part 6.2.3)
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_ModeReg_ADDRESS, 0x3D);
    if (retVal != ESP_OK)
    {
        return retVal;
    }

    return ESP_OK;
}

bool CardPresent(spi_device_handle_t *spiHandle)
{
    uint8_t irq = 0;
    MFRC522_ReadRegister(spiHandle, MFRC522_ComIrqReg_ADDRESS, &irq);
    
    if (irq & 0x30) {
        // Read the FIFO buffer here and check the received data
        uint8_t fifoData[64] = {0}; // assuming maximum 64 bytes
        for (int i = 0; i < 64; ++i) {
            MFRC522_ReadRegister(spiHandle, MFRC522_FIFODataReg_ADDRESS, &fifoData[i]);
            printf("%d: %x\n", i, fifoData[i]);
        }
        
        // Now check the received data to see if a card is present
        // This depends on the specific type of RFID card you are using
        // ...
        
        return true;
    }

    return false;
}

esp_err_t MRFC552_SelfTest (spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;

    retVal = MFRC522_SendCmd(spiHandle, MFRC522_CMDReg_ADDRESS, 0xFF); // Soft Reset

    uint8_t clearBuf[25] = {0};
    for (uint8_t i = 0; i < 25; i++)
    {
        retVal = MFRC522_WriteRegister(spiHandle, MFRC522_FIFODataReg_ADDRESS, clearBuf[i]); // Use a new function to write to register
    }

    retVal = MFRC522_WriteRegister(spiHandle, 0x36, 0x09); // Start self-test
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_FIFODataReg_ADDRESS, 0x00); // Write 00h to FIFO
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_CMDReg_ADDRESS, 0x03); // Start CalcCRC command

    uint8_t rxData[64];
    for(int i = 0; i < 64; i++)
    {
        retVal = MFRC522_ReadRegister(spiHandle, MFRC522_FIFODataReg_ADDRESS, &rxData[i]);
        printf("%x\n", rxData[i]);
    }

    return retVal;
}
