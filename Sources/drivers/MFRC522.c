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

esp_err_t MFRC522_WriteRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t value)
{
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

esp_err_t MFRC522_ReadRegisterArr (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * dataArr, uint8_t dataSize)
{
    esp_err_t retVal = ESP_OK;

    for (uint8_t i = 0; i < dataSize; i++)
    {
        retVal = MFRC522_ReadRegister(spiHandle, registerAddress, &dataArr[i]);
        if (retVal != ESP_OK)
        {
            break;
        }
    }

    return retVal;
}

esp_err_t MFRC522_WriteRegisterArr (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * dataArr, uint8_t dataSize)
{
    esp_err_t retVal = ESP_OK;

    for (uint8_t i = 0; i < dataSize; i++)
    {
        retVal = MFRC522_WriteRegister(spiHandle, registerAddress, dataArr[i]);
        if (retVal != ESP_OK)
        {
            break;
        }
    }

    return retVal;
}

esp_err_t MFRC522_ClrRegBitMask (spi_device_handle_t * spiHandle, uint8_t registerAdress, uint8_t mask)
{
    esp_err_t retVal;
    uint8_t temp;

    retVal = MFRC522_ReadRegister(spiHandle, registerAdress, &temp);
    retVal = MFRC522_WriteRegister(spiHandle, registerAdress, temp & (~mask));

    return retVal;
}

esp_err_t MFRC522_AntennaOn(spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;
    uint8_t readByte;

    retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_TX_CONTROL, &readByte);
    if ((readByte & 0x03) != 0x03)
    {
        retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_CONTROL, readByte | 0x03);
    }

    return retVal;
}

esp_err_t MFRC522_SelfTest (spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;
    uint8_t zeroBytes[25] = {0x00};

    retVal = MFRC522_Reset(spiHandle);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); // flush fifo buffer

    retVal = MFRC522_WriteRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, zeroBytes, 25); // Writing 25 bytes of zero to fifo reg

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_MEM); // stores the 25 bytes in internal buffer
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_AUTO_TEST, 0x09); // Enables autotest
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_DATA, 0x00); // Write 0 to the fifo reg
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_CALC_CRC); // start self test

    // Wait for test to complete
    uint8_t n;
    for (uint8_t i = 0; i < 0xFF; i++)
    {
        retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &n);
        if (n >= 64)
        {
            break;
        }
    }

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE); // stop calulating CRC

    uint8_t data[65];
    retVal = MFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, data, 65);

    // print resulting bytes
    for (uint8_t i = 0; i < 65; i++)
    {
        printf("%x\n", data[i]);
    }

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_AUTO_TEST, 0x00); // resetting to 0
    retVal = MFRC522_Init(spiHandle); // Re-init

    return retVal;
}

esp_err_t MFRC522_Reset (spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, 0xFF);

    return retVal;
}

esp_err_t MFRC522_SendPICCcmdTranscieve (spi_device_handle_t * spiHandle, uint8_t piccCmd)
{
    esp_err_t retVal;

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE); // stop cmds running
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMIRQ, 0x7F); // clear interupt requests
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); // flush fifo buffer
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_DATA, piccCmd); // write the cmd to the fifo buffer
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_BIT_FRAMING, 0x07); // 7 bits will be sent
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_TRANSCEIVE); // transmit the data in the fifo buffer
    // uint8_t TxIRq_bit = 0;
    // while (TxIRq_bit != 1)
    // {
    //     retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_COMIRQ, &TxIRq_bit);
    //     printf("%x\n", TxIRq_bit);
    //     TxIRq_bit = (TxIRq_bit & 0x40);
    // }

    uint8_t responce[64];
    retVal = MFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, responce, 64);

    for (uint8_t i = 0; i < 64; i++)
    {
        printf("%x\n", responce[i]);
    }
    printf("\n\n\n\n\n\n");

    return retVal;
}