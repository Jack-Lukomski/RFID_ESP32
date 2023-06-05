#include "MFRC522.h"

esp_err_t MFRC522_Init(spi_device_handle_t * spiHandle, uint8_t rstPin)
{
    esp_err_t retVal;

    gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL<<rstPin),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = 0,
    .pull_down_en = 0,
    .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&io_conf);

    if (gpio_get_level(rstPin) == 0)
    {
        gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL<<rstPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
        };

        gpio_config(&io_conf);
        gpio_set_level(rstPin, 0);
        vTaskDelay(10/portTICK_PERIOD_MS);
        gpio_set_level(rstPin, 1);
        vTaskDelay(50/portTICK_PERIOD_MS);
    }
    
    retVal = MFRC522_Reset(spiHandle);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_MODE, 0x00);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_RX_MODE, 0x00); // put back to 0x00
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_MOD_WIDTH, 0x26);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_MODE, 0x80);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_PRESCALER, 0xA9);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_RELOAD_H, 0x03);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_T_RELOAD_L, 0xE8);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_AUTO, 0x40);
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
        .length = 16,
        .tx_buffer = txData,
        .rx_buffer = rxData,
        .flags = 0,
    };

    retVal = spi_device_transmit(*spiHandle, &currTrans);

    if (retVal == ESP_OK)
    {
        *data = rxData[1];
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

esp_err_t MFRC522_SetRegBitMask(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t mask)
{
    esp_err_t retVal;
    uint8_t temp;

    retVal = MFRC522_ReadRegister(spiHandle, registerAddress, &temp);
    retVal = MFRC522_WriteRegister(spiHandle, registerAddress, temp | mask);
    
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

esp_err_t MFRC522_SelfTest (spi_device_handle_t * spiHandle, uint8_t rstPin)
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
    for (uint8_t i = 0; i < 64; i++)
    {
        printf("%x\n", data[i]);
    }

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_AUTO_TEST, 0x00); // resetting to 0
    retVal = MFRC522_Init(spiHandle, rstPin); // Re-init

    return retVal;
}

esp_err_t MFRC522_Reset (spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, 0x0F);

    uint8_t count = 0;
    uint8_t readCmdReg;
	do 
    {
		// Wait for the PowerDown bit in CommandReg to be cleared (max 3x50ms)
		vTaskDelay(50/portTICK_PERIOD_MS);
        retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_COMMAND, &readCmdReg);
	} while ((readCmdReg & (1 << 4)) && (++count) < 3);

    return retVal;
}

esp_err_t MFRC522_SendPICCcmdTranscieve (spi_device_handle_t * spiHandle, uint8_t piccCmd, uint8_t waitIrq, uint8_t * cmdBuf, uint8_t bufSize)
{
    esp_err_t retVal;
    
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_MODE, 0x00);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_RX_MODE, 0x00); 
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_MOD_WIDTH, 0x26);

    retVal = MFRC522_ClrRegBitMask(spiHandle, MFRC522_REG_COLL, 0x80);

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE); // stop cmds running
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMIRQ, 0x7F); // clear interupt requests
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); // flush fifo buffer
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_DATA, piccCmd); // write the cmd to the fifo buffer
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_BIT_FRAMING, 0x07); //test // 7 bits will be sent
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_TRANSCEIVE); // transmit the data in the fifo buffer
    retVal = MFRC522_SetRegBitMask(spiHandle, MFRC522_REG_BIT_FRAMING, 0x80);

    while(1)
    {
        uint8_t irqReg;
        retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_COMIRQ, &irqReg);

        if (waitIrq & irqReg)
        {
            // sucess
            break;
        }
        if (irqReg & 0x01)
        {
            // fail (timeout)
            break;
        }
    }

    return retVal;
}

bool MFRC522_IsCardPresent(spi_device_handle_t *spiHandle)
{
    bool retVal;
    esp_err_t espErr;
    uint8_t result;

    espErr = MFRC522_SendPICCcmdTranscieve(spiHandle, PICC_CMD_REQA, 0x30);
    espErr = MFRC522_ReadRegister(spiHandle, MFRC522_REG_COMIRQ, &result);

    if (result & 0x30)
    {
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return retVal;
}

UniqueIdentifier_t * MFRC522_ReadUID(spi_device_handle_t *spiHandle, uidSize_t uidSize)
{
    UniqueIdentifier_t * newUidRead = (newUidRead*) malloc(sizeof(UniqueIdentifier_t));
    esp_err_t espErr;

    newUidRead->uidSize = uidSize;

    switch (uidSize)
    {
    case singleSizeUID_t:
        newUidRead->uidData.singleSizeUidData = {0x00};
        break;
    case singleSizeUID_t:
        newUidRead->uidData.doubleSizeUidData = {0x00};
        break;
    case singleSizeUID_t:
        newUidRead->uidData.trippleSizeUidData = {0x00};
        break;
    default:
        break;
    }    

    espErr = MFRC522_SendPICCcmdTranscieve(spiHandle, )
}