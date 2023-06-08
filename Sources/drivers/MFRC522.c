#include "MFRC522.h"

static const uint8_t MIFARE_DEFAULT_BLOCK_KEY[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

esp_err_t xMFRC522_Init(spi_device_handle_t * spiHandle, uint8_t rstPin)
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
    
    retVal = xMFRC522_Reset(spiHandle);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_MODE, 0x00);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_RX_MODE, 0x00); // put back to 0x00
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_MOD_WIDTH, 0x26);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_T_MODE, 0x80);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_T_PRESCALER, 0xA9);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_T_RELOAD_H, 0x03);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_T_RELOAD_L, 0xE8);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_AUTO, 0x40);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_MODE, 0x3D);
    xMFRC522_AntennaOn(spiHandle);

    return retVal;
}

esp_err_t xMFRC522_WriteRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t value)
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

esp_err_t xMFRC522_ReadRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * data)
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

esp_err_t xMFRC522_ReadRegisterArr (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * dataArr, uint8_t dataSize)
{
    esp_err_t retVal = ESP_OK;

    for (uint8_t i = 0; i < dataSize; i++)
    {
        retVal = xMFRC522_ReadRegister(spiHandle, registerAddress, &dataArr[i]);
        if (retVal != ESP_OK)
        {
            break;
        }
    }

    return retVal;
}

esp_err_t xMFRC522_WriteRegisterArr (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * dataArr, uint8_t dataSize)
{
    esp_err_t retVal = ESP_OK;

    for (uint8_t i = 0; i < dataSize; i++)
    {
        retVal = xMFRC522_WriteRegister(spiHandle, registerAddress, dataArr[i]);
        if (retVal != ESP_OK)
        {
            break;
        }
    }

    return retVal;
}

esp_err_t xMFRC522_ClrRegBitMask (spi_device_handle_t * spiHandle, uint8_t registerAdress, uint8_t mask)
{
    esp_err_t retVal;
    uint8_t temp;

    retVal = xMFRC522_ReadRegister(spiHandle, registerAdress, &temp);
    retVal = xMFRC522_WriteRegister(spiHandle, registerAdress, temp & (~mask));

    return retVal;
}

esp_err_t xMFRC522_SetRegBitMask(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t mask)
{
    esp_err_t retVal;
    uint8_t temp;

    retVal = xMFRC522_ReadRegister(spiHandle, registerAddress, &temp);
    retVal = xMFRC522_WriteRegister(spiHandle, registerAddress, temp | mask);
    
    return retVal;
}

esp_err_t xMFRC522_AntennaOn(spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;
    uint8_t readByte;

    retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_TX_CONTROL, &readByte);
    if ((readByte & 0x03) != 0x03)
    {
        retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_CONTROL, readByte | 0x03);
    }

    return retVal;
}

esp_err_t xMFRC522_SelfTest (spi_device_handle_t * spiHandle, uint8_t rstPin)
{
    esp_err_t retVal;
    uint8_t zeroBytes[25] = {0x00};

    retVal = xMFRC522_Reset(spiHandle);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); // flush fifo buffer

    retVal = xMFRC522_WriteRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, zeroBytes, 25); // Writing 25 bytes of zero to fifo reg

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_MEM); // stores the 25 bytes in internal buffer
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_AUTO_TEST, 0x09); // Enables autotest
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_DATA, 0x00); // Write 0 to the fifo reg
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_CALC_CRC); // start self test

    // Wait for test to complete
    uint8_t n;
    for (uint8_t i = 0; i < 0xFF; i++)
    {
        retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &n);
        if (n >= 64)
        {
            break;
        }
    }

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE); // stop calulating CRC

    uint8_t data[65];
    retVal = xMFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, data, 65);

    // print resulting bytes
    for (uint8_t i = 0; i < 64; i++)
    {
        printf("%x\n", data[i]);
    }

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_AUTO_TEST, 0x00); // resetting to 0
    retVal = xMFRC522_Init(spiHandle, rstPin); // Re-init

    return retVal;
}

esp_err_t xMFRC522_Reset (spi_device_handle_t * spiHandle)
{
    esp_err_t retVal;

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, 0x0F);

    uint8_t count = 0;
    uint8_t readCmdReg;
	do 
    {
		// Wait for the PowerDown bit in CommandReg to be cleared (max 3x50ms)
		vTaskDelay(50/portTICK_PERIOD_MS);
        retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_COMMAND, &readCmdReg);
	} while ((readCmdReg & (1 << 4)) && (++count) < 3);

    return retVal;
}

esp_err_t xMFRC522_Transcieve(spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t * cmdBuf, uint8_t bufSize, bitFraming_t bitFrame)
{
    return xMFRC522_CommWithMifare(PCD_CMD_TRANSCEIVE, spiHandle, waitIrq, cmdBuf, bufSize, bitFrame);
}

esp_err_t xMFRC522_MF_Authent(spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t * cmdBuf, uint8_t bufSize, bitFraming_t bitFrame)
{
    return xMFRC522_CommWithMifare(PCD_CMD_MF_AUTHENT, spiHandle, waitIrq, cmdBuf, bufSize, bitFrame);
}

esp_err_t xMFRC522_CommWithMifare(uint8_t cmd, spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t * cmdBuf, uint8_t bufSize, bitFraming_t bitFrame)
{
    esp_err_t retVal;
    
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_TX_MODE, 0x00);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_RX_MODE, 0x00); 
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_MOD_WIDTH, 0x26);

    // retVal = MFRC522_ClrRegBitMask(spiHandle, MFRC522_REG_COLL, 0x80);

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE); // stop cmds running
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMIRQ, 0x7F); // clear interupt requests
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); // flush fifo buffer

    for (uint8_t currCmd = 0; currCmd < bufSize; currCmd++)
    {
        retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_DATA, cmdBuf[currCmd]); // write the cmd to the fifo buffer
    }

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_BIT_FRAMING, bitFrame); //test // 7 bits will be sent
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, cmd); // transmit the data in the fifo buffer
    if (cmd == PCD_CMD_TRANSCEIVE)
    {
        retVal = xMFRC522_SetRegBitMask(spiHandle, MFRC522_REG_BIT_FRAMING, 0x80); // start transmission
    }

    while(1)
    {
        uint8_t irqReg;
        retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_COMIRQ, &irqReg);

        if (waitIrq & irqReg)
        {
            retVal = ESP_OK;
            break;
        }
        if (irqReg & 0x01)
        {
            retVal = ESP_FAIL;
            break;
        }
    }

    return retVal;
}

bool xMFRC522_IsCardPresent(spi_device_handle_t *spiHandle)
{
    bool retVal;
    esp_err_t espErr;
    uint8_t result;
    uint8_t cmdBuf[1] = {PICC_CMD_REQA};

    espErr = xMFRC522_Transcieve(spiHandle, 0x30, cmdBuf, 1, sevenBit);
    espErr = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_COMIRQ, &result);

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

esp_err_t xMFRC522_CalculateCRC(spi_device_handle_t *spiHandle, uint8_t * buf, uint8_t bufLen, uint8_t resultBuf[2])
{
    esp_err_t retVal;
    uint8_t n;

    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_DIVIRQ, 0x04); // clr the CRCIRq flag
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, 0x80); 
    retVal = xMFRC522_WriteRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, buf, bufLen);
    retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_CALC_CRC);
    
    while(1)
    {
        retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_DIVIRQ, &n);

        if (n & 0x04)
        {
            retVal = xMFRC522_WriteRegister(spiHandle, MFRC522_REG_COMMAND, PCD_CMD_IDLE);
            retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_CRC_RESULT_L, &resultBuf[0]);
            retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_CRC_RESULT_M, &resultBuf[1]);
            break; 
        }
    }

    return retVal;
}

UniqueIdentifier_t * xMifare_ReadUID(spi_device_handle_t *spiHandle, uidSize_t uidSize)
{
    UniqueIdentifier_t * newUidRead = (UniqueIdentifier_t*) malloc(sizeof(UniqueIdentifier_t));
    esp_err_t espErr;
    uint8_t buf[2];
    uint8_t fifoDataCount;

    newUidRead->uidSize = uidSize;

    buf[0] = PICC_CMD_SEL_CL1;
    buf[1] = 0x20;
    espErr = xMFRC522_Transcieve(spiHandle, 0x30, buf, 2, eightBit);

    espErr = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &fifoDataCount);
    uint8_t * fifoData = (uint8_t*) malloc(sizeof(uint8_t) * fifoDataCount);
    espErr = xMFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, fifoData, fifoDataCount);

    if (xPrv_Mifare_BlockCheckChar(fifoData, fifoDataCount, newUidRead))
    {
        switch (uidSize)
        {
        case fourBytesSingle:
            memcpy(newUidRead->uidData.singleSizeUidData, fifoData, 4);
            break;
        case sevenBytesDouble:
            memcpy(newUidRead->uidData.doubleSizeUidData, fifoData, 7);
            break;
        case tenBytesTripple:
            memcpy(newUidRead->uidData.doubleSizeUidData, fifoData, 10);
            break;
        default:
            break;
        }
        vMifare_PrintUID(newUidRead);
        espErr = xMifare_SetSakByte(spiHandle, newUidRead);
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

static bool xPrv_Mifare_BlockCheckChar(uint8_t * bufData, uint8_t bufSize, UniqueIdentifier_t * UID)
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

static esp_err_t xMifare_SetSakByte(spi_device_handle_t *spiHandle, UniqueIdentifier_t * UID)
{
    esp_err_t retVal;
    uint8_t cmd[9];
    uint8_t sak[3];

    cmd[0] = PICC_CMD_SEL_CL1;
    cmd[1] = 0x70;
    cmd[2] = UID->uidData.singleSizeUidData[0];
    cmd[3] = UID->uidData.singleSizeUidData[1];
    cmd[4] = UID->uidData.singleSizeUidData[2];
    cmd[5] = UID->uidData.singleSizeUidData[3];
    cmd[6] = UID->bccByte;

    retVal = xMFRC522_CalculateCRC(spiHandle, cmd, 7, &cmd[7]); 
    retVal = xMFRC522_Transcieve(spiHandle, 0x30, cmd, sizeof(cmd), eightBit);
    printf("responce to sel\n");
    vMFRC522_GetAndPrintFifoBuf(spiHandle, sak, true);

    UID->sakByte = sak[0];

    return retVal;

}

esp_err_t xMifare_Authenticate(spi_device_handle_t *spiHandle, uint8_t cmd, uint8_t blockAddress, uint8_t * key, UniqueIdentifier_t * UID)
{
    esp_err_t espErr = ESP_OK;
    uint8_t status;
    uint8_t waitIrq = 0x10;
    uint8_t sendBuf[12];

    sendBuf[0] = cmd;
    sendBuf[1] = blockAddress;

    for (uint8_t i = 0; i < MIFARE_KEY_SIZE; i++)
    {
        sendBuf[i + 2] = key[i]; 
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        sendBuf[i + 8] = UID->uidData.singleSizeUidData[i];
    }

    espErr = xMFRC522_MF_Authent(spiHandle, waitIrq, sendBuf, sizeof(sendBuf), eightBit);

    return espErr;
}

esp_err_t xMifare_ReadKeyBlock(spi_device_handle_t *spiHandle, uint8_t blockAddress, UniqueIdentifier_t * UID)
{
    esp_err_t retVal;
    uint8_t sendBuf[4];

    sendBuf[0] = PICC_CMD_MF_READ;
    sendBuf[1] = blockAddress;
    retVal = xMFRC522_CalculateCRC(spiHandle, sendBuf, 2, &sendBuf[2]);

    retVal = xMifare_Authenticate(spiHandle, PICC_CMD_MF_AUTH_KEY_A, blockAddress, MIFARE_DEFAULT_BLOCK_KEY, UID);
    retVal = xMFRC522_Transcieve(spiHandle, 0x30, sendBuf, sizeof(sendBuf), eightBit);

    return retVal;
}

Mifare1kKey_t * xMifare_GetKeyData(spi_device_handle_t *spiHandle, UniqueIdentifier_t * UID)
{
    esp_err_t retVal;
    Mifare1kKey_t * newKey = (Mifare1kKey_t*) malloc(sizeof(Mifare1kKey_t));

    newKey->uid = *UID;

    for (uint8_t currSector = 0; currSector < NUM_SECTORE_MIFARE_1K; currSector++)
    {
        for (uint8_t currBlock = 0; currBlock < NUM_BLOCKS_PER_SECTOR; currBlock++)
        {
            uint8_t blockAddress = currSector * NUM_BLOCKS_PER_SECTOR + currBlock;
  
            retVal = xMifare_ReadKeyBlock(spiHandle, blockAddress, UID);
            retVal = xMFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, &newKey->keyData[currSector][currBlock*16], 16);
        }
    }

    return newKey;
}

esp_err_t xMifare_WriteKeyBlock(spi_device_handle_t *spiHandle, uint8_t blockAddress, UniqueIdentifier_t * UID, uint8_t data[16])
{
    esp_err_t retVal;
    uint8_t sendBuf[4];
    uint8_t sendData[18];
    uint8_t ack;

    if ((blockAddress+1%4) == 0 || blockAddress < 4)
    {
        return ESP_FAIL;
    }

    sendBuf[0] = PICC_CMD_MF_WRITE;
    sendBuf[1] = blockAddress;
    retVal = xMFRC522_CalculateCRC(spiHandle, sendBuf, 2, &sendBuf[2]);

    retVal = xMifare_Authenticate(spiHandle, PICC_CMD_MF_AUTH_KEY_A, blockAddress, MIFARE_DEFAULT_BLOCK_KEY, UID);
    retVal = xMFRC522_Transcieve(spiHandle, 0x30, sendBuf, 4, eightBit);

    retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_DATA, &ack);

    if (ack != MFRC522_MIFARE_ACK)
    {
        return ESP_FAIL;
    }

    memcpy(sendData, data, 16);
    retVal = xMFRC522_CalculateCRC(spiHandle, sendData, 16, &sendData[16]);

    retVal = xMFRC522_Transcieve(spiHandle, 0x30, sendData, 18, eightBit);
    retVal = xMFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_DATA, &ack);

    if (ack != MFRC522_MIFARE_ACK)
    {
        return ESP_FAIL;
    }

    return retVal;
}

esp_err_t xMifare_WriteKey(spi_device_handle_t *spiHandle, UniqueIdentifier_t * UID, uint8_t data[45][16])
{
    uint8_t keyIndex = 0;

    // Skip the first sector as it is the manufacturer's sector
    for (uint8_t sector = 1; sector < 16; sector++)
    {
        for (uint8_t block = 0; block < 3; block++) // Skip the last block as it contains sensitive data
        {
            uint8_t blockAddress = sector * 4 + block;
            esp_err_t result = xMifare_WriteKeyBlock(spiHandle, blockAddress, UID, data[keyIndex]);
            printf("Block address %d\n", blockAddress);
            keyIndex++;
        }
    }

    return ESP_OK;
    return ESP_OK;
}

void vMifare_PrintUID(UniqueIdentifier_t * UID)
{
    printf("<--- Tag's UID --->\n");
    for (uint8_t i = 0; i < 4; i++)
    {
        printf("Byte %d: %x\n", i, UID->uidData.singleSizeUidData[i]);
    }
    printf("BCC Byte: %x\n", UID->bccByte);
}

void vMFRC522_GetAndPrintFifoBuf(spi_device_handle_t *spiHandle, uint8_t * fifoBuf, bool print)
{
    uint8_t fifoSize;

    xMFRC522_ReadRegister(spiHandle, MFRC522_REG_FIFO_LEVEL, &fifoSize);
    fifoBuf = (uint8_t *) malloc(sizeof(uint8_t) * fifoSize);
    xMFRC522_ReadRegisterArr(spiHandle, MFRC522_REG_FIFO_DATA, fifoBuf, fifoSize);

    if (print == true)
    {
        printf("<--- FIFO Buf Contents --->\n");
        printf("Size: %d\n", fifoSize);
        for (uint8_t i = 0; i < fifoSize; i++)
        {
            printf("FIFO %d: %x\n", i, fifoBuf[i]);
        }
    }
}

void vMifare_PrintKey(Mifare1kKey_t * key)
{
    printf("Sector   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15\n");
    for (uint8_t currSector = 0; currSector < 16; currSector++)
    {
        printf("   %d\t", currSector);
        for (uint8_t currBlocks = 0; currBlocks < 64; currBlocks++)
        {
            if (currBlocks % 16 == 0)
            {
                printf("\n\t");
            }
            printf(" %02x ", key->keyData[currSector][currBlocks]);
        }
        printf("\n");
    }
}
