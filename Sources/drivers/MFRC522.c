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

bool MFRC522_IsNewCardPresent(spi_device_handle_t *spiHandle)
{
    esp_err_t retVal;

    // Enable the antenna
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_TxControlReg_ADDRESS, 0x03);
    if (retVal != ESP_OK)
    {
        return false;
    }

    // Send the Request command
    retVal = MFRC522_SendCmd(spiHandle, MFRC522_FIFOLevelReg_ADDRESS, 0x80); // clear the FIFO data
    if (retVal != ESP_OK)
    {
        return false;
    }

    retVal = MFRC522_SendCmd(spiHandle, MFRC522_FIFODataReg_ADDRESS, 0x26); // write the command to FIFO
    if (retVal != ESP_OK)
    {
        return false;
    }

    retVal = MFRC522_SendCmd(spiHandle, MFRC522_CMDReg_ADDRESS, 0x0C); // execute the command
    if (retVal != ESP_OK)
    {
        return false;
    }

    retVal = MFRC522_SendCmd(spiHandle, MFRC522_BitFramingReg_ADDRESS, 0x87); // StartSend=1, transmission of data starts, last bits are given in BitFramingReg_Register
    if (retVal != ESP_OK)
    {
        return false;
    }

    // After executing the command, the status register will be updated
    uint8_t status;
    retVal = MFRC522_ReadRegister(spiHandle, MFRC522_Status2Reg_ADDRESS, &status);
    printf("%d\n", status);
    if (retVal == ESP_OK && (status & 0x07))
    {
        // The lower bits of the Status2Reg register will be non-zero if a card responded
        return true;
    }
    return false;
}

