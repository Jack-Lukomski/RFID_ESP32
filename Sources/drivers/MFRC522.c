#include "MFRC522.h"

static const uint8_t MIFARE_DEFAULT_BLOCK_KEY[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

esp_err_t MFRC522_SendPICCcmdTranscieve (spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t * cmdBuf, uint8_t bufSize, bitFraming_t bitFrame)
{
    esp_err_t retVal;
    
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_MODE, 0x00);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_RX_MODE, 0x00); 
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_MOD_WIDTH, 0x26);

    // retVal = MFRC522_ClrRegBitMask(spiHandle, MFRC522_REG_COLL, 0x80);

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE); // stop cmds running
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMIRQ, 0x7F); // clear interupt requests
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); // flush fifo buffer

    for (uint8_t currCmd = 0; currCmd < bufSize; currCmd++)
    {
        retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_DATA, cmdBuf[currCmd]); // write the cmd to the fifo buffer
    }

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_BIT_FRAMING, bitFrame); //test // 7 bits will be sent
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
    uint8_t cmdBuf[1] = {PICC_CMD_REQA};

    espErr = MFRC522_SendPICCcmdTranscieve(spiHandle, 0x30, cmdBuf, 1, sevenBit);
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

esp_err_t MFRC522_CalculateCRC(spi_device_handle_t *spiHandle, uint8_t * buf, uint8_t bufLen, uint8_t resultBuf[2])
{
    esp_err_t retVal;
    uint8_t n;

    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_DIVIRQ, 0x04); // clr the CRCIRq flag
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); 
    retVal = MFRC522_WriteRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, buf, bufLen);
    retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_CALC_CRC);
    
    while(1)
    {
        retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_DIVIRQ, &n);

        if (n & 0x04)
        {
            retVal = MFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE);
            retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_CRC_RESULT_L, &resultBuf[0])
            retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_CRC_RESULT_M, &resultBuf[1]);
            break; 
        }
    }

    return retVal;
}

UniqueIdentifier_t * MFRC522_ReadUID(spi_device_handle_t *spiHandle, uidSize_t uidSize)
{
    UniqueIdentifier_t * newUidRead = (UniqueIdentifier_t*) malloc(sizeof(UniqueIdentifier_t));
    esp_err_t espErr;
    uint8_t buf[2];
    uint8_t fifoDataCount;

    newUidRead->uidSize = uidSize;

    buf[0] = PICC_CMD_SEL_CL1;
    buf[1] = 0x20;
    espErr = MFRC522_SendPICCcmdTranscieve(spiHandle, 0x30, buf, 2, eightBit);

    espErr = MFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &fifoDataCount);
    uint8_t * fifoData = (uint8_t*) malloc(sizeof(uint8_t) * fifoDataCount);
    espErr = MFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, fifoData, fifoDataCount);

    if (UID_BlockCheckChar(fifoData, fifoDataCount, newUidRead))
    {
        switch (uidSize)
        {
        case fourBytesSingle:
            memcpy(newUidRead->uidData.singleSizeUidData, fifoData, 3);
            break;
        case sevenBytesDouble:
            memcpy(newUidRead->uidData.doubleSizeUidData, fifoData, 6);
            break;
        case tenBytesTripple:
            memcpy(newUidRead->uidData.doubleSizeUidData, fifoData, 9);
            break;
        default:
            break;
        }
        MFRC522_PrintUID(newUidRead);
        // espErr = MFRC522_SetSakByte(spiHandle, newUidRead);
    }
    else
    {
        return NULL;
    }

    if (espErr != ESP_OK)
    {
        return NULL;
    }

    return newUidRead;
}

static bool UID_BlockCheckChar (uint8_t * bufData, uint8_t bufSize, UniqueIdentifier_t * UID)
{
    bool retVal;
    uint8_t xorResult = 0;

    for (uint8_t currByte = 0; currByte < bufSize-1; currByte++)
    {
        xorResult = xorResult ^ bufData[currByte];
    }

    UID->bccByte = bufData[bufSize-1];

    if (xorResult == bufData[bufSize-1])
    {
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return retVal;
}

// static esp_err_t MFRC522_SetSakByte(spi_device_handle_t *spiHandle, UniqueIdentifier_t * UID)
// {
//     esp_err_t retVal;
//     uint8_t recieveBuf[3];
//     uint8_t sakCmdBuf[7] = {PICC_CMD_SEL_CL1, 0x70, UID->uidData.singleSizeUidData[0], 
//               UID->uidData.singleSizeUidData[1], UID->uidData.singleSizeUidData[2], 
//               UID->uidData.singleSizeUidData[3], UID->bccByte};
    
//     retVal = MFRC522_SendPICCcmdTranscieve(spiHandle, 0x30, sakCmdBuf, sizeof(sakCmdBuf), eightBit);
//     retVal = MFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, recieveBuf, sizeof(recieveBuf));

//     UID->sakByte = recieveBuf[0];

//     printf("%x\n", recieveBuf[0]);
//     printf("%x\n", recieveBuf[1]);
//     printf("%x\n", recieveBuf[2]);

//     return retVal;
// }

Mifare1kKey_t * MFRC522_GetKeyData(spi_device_handle_t * spiHandle, UniqueIdentifier_t * UID)
{
    Mifare1kKey_t * key = (Mifare1kKey_t*) malloc(sizeof(Mifare1kKey_t));

    memcpy(key->uid.uidData.singleSizeUidData, UID->uidData.singleSizeUidData, 3);
    memcpy(key->blockKey, MIFARE_DEFAULT_BLOCK_KEY, 5);

    for (uint8_t currSector = 0; currSector < NUM_SECTORE_MIFARE_1K; currSector++)
    {
        printf("Sector %d\n", currSector);
        for (uint8_t currBlock = 0; currBlock < NUM_BLOCKS_PER_SECTOR; currBlock++)
        {
            uint8_t currBlockData[16];
            uint8_t currBlockAddr = (currSector == 0) ? currBlock : ((currSector - 1) * 4) + 4 + currBlock;

            MFRC522_Authenticate(spiHandle, PICC_CMD_MF_AUTH_KEY_A, currBlockAddr, key->blockKey, UID);
            MFRC522_ReadKeyBlock(spiHandle, currBlockAddr, currBlockData, sizeof(currBlockData));

            printf("Block %d\n", currBlock);
            printf("Curr block data:\n");
            for (uint8_t i = 0; i < 16; i++)
            {
                printf("%x\n", currBlockData[i]);
            }
            printf("\n\n");
        }
    }

    return NULL;
}

esp_err_t MFRC522_ReadKeyBlock(spi_device_handle_t *spiHandle, uint8_t blockAddress, uint8_t * buf, uint8_t bufSize)
{
    esp_err_t retVal;
    uint8_t fifoDataCount;
    uint8_t cmdBuf[2];

    cmdBuf[0] = PICC_CMD_MF_READ;
    cmdBuf[1] = blockAddress;

    retVal = MFRC522_SendPICCcmdTranscieve(spiHandle, 0x30, cmdBuf, 2, eightBit);
    retVal = MFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &fifoDataCount);

    if (fifoDataCount > bufSize)
    {
        return ESP_FAIL;
    }

    retVal = MFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, buf, fifoDataCount);

    return retVal;
}

esp_err_t MFRC522_Authenticate(spi_device_handle_t *spiHandle, uint8_t cmd, uint8_t blockAddress, uint8_t * key, UniqueIdentifier_t * UID)
{
    esp_err_t espErr;
    uint8_t status;
    uint8_t waitIrq = 0x30;
    uint8_t sendBuf[12];

    sendBuf[0] = cmd;
    sendBuf[1] = blockAddress;

    for (uint8_t i = 0; i < MIFARE_KEY_SIZE; i++)
    {
        sendBuf[i + 2] = key[i]; 
    }

    // Adding four uid bytes to buf
    // for (uint8_t i = 0; i < 4; i++)
    // {
    //     sendBuf[i + 8] = UID->uidData.singleSizeUidData[i];
    // }

    sendBuf[8] = UID->uidData.singleSizeUidData[3];
    sendBuf[9] = UID->uidData.singleSizeUidData[2];
    sendBuf[10] = UID->uidData.singleSizeUidData[1];
    sendBuf[11] = UID->uidData.singleSizeUidData[0];

    // espErr = MFRC522_SendPICCcmdTranscieve(spiHandle, waitIrq, sendBuf, sizeof(sendBuf), eightBit);
    // espErr = MFRC522_ReadRegister(spiHandle, MFRC522_REG_STATUS2, &status);

    return espErr;
}

void MFRC522_PrintUID(UniqueIdentifier_t * UID)
{
    printf("<--- Tag's UID --->\n");
    for (uint8_t i = 0; i < 4; i++)
    {
        printf("Byte %d: %x\n", i, UID->uidData.singleSizeUidData[i]);
    }
    printf("BCC Byte: %x\n", UID->bccByte);
}

void MFRC522_GetAndPrintFifoBuf(spi_device_handle_t *spiHandle, uint8_t * fifoBuf, bool print)
{
    uint8_t fifoSize;

    MFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &fifoSize);
    fifoBuf = (uint8_t *) malloc(sizeof(uint8_t) * fifoSize);
    MFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, fifoBuf, fifoSize);

    if (print == true)
    {
        printf("<--- FIFO Buf Contents --->\n");
        printf("Size: %x\n", fifoSize);
        for (uint8_t i = 0; i < fifoSize; i++)
        {
            printf("FIFO %d: %x\n", i, fifoBuf[i]);
        }
    }
}