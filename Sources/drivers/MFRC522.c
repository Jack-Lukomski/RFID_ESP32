#include "MFRC522.h"

esp_err_t MFRC522_Init(spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_MODE, 0x00);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_RX_MODE, 0x00);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_MOD_WIDTH, 0x26);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_MODE, 0x80);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_PRESCALER, 0xA9);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_RELOAD_H, 0x03);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_RELOAD_L, 0xE8);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_AUTO, 0x00);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_MODE, 0x3D);
    MFRC522_AntennaOn(spiHandle);

    return retVal;
}

esp_err_t MFRC522_WriteRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t value){
    esp_err_t retVal;
    uint8_t txData[2] = {(registerAddress & 0x7F), value};

    spi_transaction_t currTrans = {
        .length = 16,
        .tx_buffer = txData,
        .flags = 0,
    };

    retVal = spi_device_transmit(*spiHandle, &currTrans);

    return retVal;
}

esp_err_t MFRC522_ReadRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * data)
{
    esp_err_t retVal;
    uint8_t txData[2] = {(registerAddress | 0x80), 0x00};
    uint8_t rxData[2];

    spi_transaction_t currTrans = {
        .length = 8,
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

void MFRC522_AntennaOn(spi_device_handle_t * spiHandle)
{
    uint8_t readByte;
    MFRC522_ReadRegister(spiHandle, MFRC522_REG_TX_CONTROL, &readByte);
    if ((readByte & 0x03) != 0x03)
    {
        MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_CONTROL, readByte | 0x03);
    }
}

bool MFRC522_IsNewCardPresent(spi_device_handle_t * spiHandle)
{
    uint8_t bufferATQA[2];
    uint8_t bufferSize = sizeof(bufferATQA);
    esp_err_t status;

    // Send REQA command
    status = MFRC522_ToCard(spiHandle, PICC_CMD_REQA, bufferATQA, &bufferSize);

    // Return true if the status is ESP_OK or STATUS_COLLISION
    // Otherwise return false
    return (status == ESP_OK);
}


esp_err_t MFRC522_ToCard(spi_device_handle_t * spiHandle, uint8_t command, uint8_t *sendData, uint8_t *sendLen) 
{
    esp_err_t status;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    
    switch (command) {
        case PICC_CMD_AUTH_KEY_A:
        case PICC_CMD_AUTH_KEY_B:
            irqEn = 0x12;
            waitIRq = 0x10;
            break;

        case PCD_CMD_TRANSCEIVE:
            irqEn = 0x77;
            waitIRq = 0x30;
            break;

        default:
            break;
    }

    // Write command to CommandReg
    MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, command);

    // Start send command
    status = MFRC522_TransceiveData(spiHandle, sendData, sendLen);

    if (status != ESP_OK) {
        return status;
    }

    return ESP_OK;
}