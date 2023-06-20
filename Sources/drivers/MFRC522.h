/**
 * @file MFRC522.h
 */

#ifndef _MFRC522_H_
#define _MFRC522_H_

#include "../unit_tests/unit_tests.h"
#include "driver/spi_master.h"
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// Number of bytes in the UID. 4, 7 or 10.
/** @typedef singleSizeUID_t
 *  @brief Typedef for single size UID. It consists of 4 bytes.
 */
typedef uint8_t singleSizeUID_t[4];

/** @typedef doubleSizeUID_t
 *  @brief Typedef for double size UID. It consists of 7 bytes.
 */
typedef uint8_t doubleSizeUID_t[7];

/** @typedef trippleSizeUID_t
 *  @brief Typedef for triple size UID. It consists of 10 bytes.
 */
typedef uint8_t trippleSizeUID_t[10];

/** @typedef bitFraming_t
 *  @brief Enum typedef for bit framing. It can either be sevenBit or eightBit.
 */
typedef enum {
    sevenBit = 0x07,    /**< 7 bit framing */
    eightBit = 0x08,    /**< 8 bit framing */
} bitFraming_t;

/** @typedef uidSize_t
 *  @brief Enum typedef for UID size. It can be either fourBytesSingle, sevenBytesDouble or tenBytesTripple.
 */
typedef enum {
    fourBytesSingle = 4,    /**< UID of 4 bytes */
    sevenBytesDouble = 7,   /**< UID of 7 bytes */
    tenBytesTripple = 10,   /**< UID of 10 bytes */
} uidSize_t;

/** @typedef UniqueIdentifier_t
 *  @brief Struct typedef to represent a unique identifier (UID).
 */
typedef struct {
    uidSize_t uidSize;    /**< The size of the UID. */
    union {
        singleSizeUID_t singleSizeUidData;    /**< Data for a single size UID. */
        doubleSizeUID_t doubleSizeUidData;    /**< Data for a double size UID. */
        trippleSizeUID_t trippleSizeUidData;  /**< Data for a triple size UID. */
    } uidData;
    uint8_t sakByte;    /**< The SAK (Select acknowledge) byte returned from the PICC after successful selection. */
    uint8_t bccByte;    /**< BCC byte. */
} UniqueIdentifier_t;

/** @typedef Mifare1kKey_t
 *  @brief Struct typedef to represent a MIFARE 1k RFID tag.
 */
typedef struct {
    UniqueIdentifier_t uid;    /**< 7 byte uid, the default */
    uint8_t blockKey[6];    /**< Block key. */
    uint8_t keyData[16][64];    /**< Key data for 1k card (1024 bytes). */
} Mifare1kKey_t;

// Definitions for some constants
#define NUM_SECTORE_MIFARE_1K 16    /**< Number of sectors in MIFARE 1k. */
#define NUM_BLOCKS_PER_SECTOR 4    /**< Number of blocks per sector. */
#define MIFARE_KEY_SIZE 6    /**< Size of MIFARE key. */

/**
 * @defgroup MFRC522_Register_Addresses MFRC522 Register Addresses
 * @{
 */
#define MFRC522_REG_RESERVED00           0x00 << 1
#define MFRC522_REG_COMMAND              0x01 << 1
#define MFRC522_REG_COMIEN               0x02 << 1
#define MFRC522_REG_DIVIEN               0x03 << 1
#define MFRC522_REG_COMIRQ               0x04 << 1
#define MFRC522_REG_DIVIRQ               0x05 << 1
#define MFRC522_REG_ERROR                0x06 << 1
#define MFRC522_REG_STATUS1              0x07 << 1
#define MFRC522_REG_STATUS2              0x08 << 1
#define MFRC522_REG_FIFO_DATA            0x09 << 1
#define MFRC522_REG_FIFO_LEVEL           0x0A << 1
#define MFRC522_REG_WATER_LEVEL          0x0B << 1
#define MFRC522_REG_CONTROL              0x0C << 1
#define MFRC522_REG_BIT_FRAMING          0x0D << 1
#define MFRC522_REG_COLL                 0x0E << 1
#define MFRC522_REG_RESERVED01           0x0F << 1

// Page 1: Command
#define MFRC522_REG_RESERVED10           0x10 << 1
#define MFRC522_REG_MODE                 0x11 << 1
#define MFRC522_REG_TX_MODE              0x12 << 1
#define MFRC522_REG_RX_MODE              0x13 << 1
#define MFRC522_REG_TX_CONTROL           0x14 << 1
#define MFRC522_REG_TX_AUTO              0x15 << 1
#define MFRC522_REG_TX_SEL               0x16 << 1
#define MFRC522_REG_RX_SEL               0x17 << 1
#define MFRC522_REG_RX_THRESHOLD         0x18 << 1
#define MFRC522_REG_DEMOD                0x19 << 1
#define MFRC522_REG_RESERVED11           0x1A << 1
#define MFRC522_REG_RESERVED12           0x1B << 1
#define MFRC522_REG_MIFARE               0x1C << 1
#define MFRC522_REG_RESERVED13           0x1D << 1
#define MFRC522_REG_RESERVED14           0x1E << 1
#define MFRC522_REG_SERIALSPEED          0x1F << 1

// Page 2: Configuration
#define MFRC522_REG_RESERVED20           0x20 << 1
#define MFRC522_REG_CRC_RESULT_M         0x21 << 1
#define MFRC522_REG_CRC_RESULT_L         0x22 << 1
#define MFRC522_REG_RESERVED21           0x23 << 1
#define MFRC522_REG_MOD_WIDTH            0x24 << 1
#define MFRC522_REG_RESERVED22           0x25 << 1
#define MFRC522_REG_RF_CFG               0x26 << 1
#define MFRC522_REG_GS_N                 0x27 << 1
#define MFRC522_REG_CWGS_PREG            0x28 << 1
#define MFRC522_REG_MOD_GS_PREG          0x29 << 1
#define MFRC522_REG_T_MODE               0x2A << 1
#define MFRC522_REG_T_PRESCALER          0x2B << 1
#define MFRC522_REG_T_RELOAD_H           0x2C << 1
#define MFRC522_REG_T_RELOAD_L           0x2D << 1
#define MFRC522_REG_T_COUNTER_VALUE_H    0x2E << 1
#define MFRC522_REG_T_COUNTER_VALUE_L    0x2F << 1

// Page 3: Test
#define MFRC522_REG_RESERVED30           0x30 << 1
#define MFRC522_REG_TEST_SEL1            0x31 << 1
#define MFRC522_REG_TEST_SEL2            0x32 << 1
#define MFRC522_REG_TEST_PIN_EN          0x33 << 1
#define MFRC522_REG_TEST_PIN_VALUE       0x34 << 1
#define MFRC522_REG_TEST_BUS             0x35 << 1
#define MFRC522_REG_AUTO_TEST            0x36 << 1
#define MFRC522_REG_VERSION              0x37 << 1
#define MFRC522_REG_ANALOG_TEST          0x38 << 1
#define MFRC522_REG_TEST_DAC1            0x39 << 1
#define MFRC522_REG_TEST_DAC2            0x3A << 1
#define MFRC522_REG_TEST_ADC             0x3B << 1
#define MFRC522_REG_RESERVED31           0x3C << 1
#define MFRC522_REG_RESERVED32           0x3D << 1
#define MFRC522_REG_RESERVED33           0x3E << 1
#define MFRC522_REG_RESERVED34           0x3F << 1
/** @} */

/**
 * @defgroup MFRC522_Commands MFRC522 Commands
 * @{
 */
// Commands sent to the PICC.
typedef enum {
    PICC_CMD_REQA         = 0x26,
    PICC_CMD_WUPA         = 0x52,
    PICC_CMD_CT           = 0x88,
    PICC_CMD_SEL_CL1      = 0x93,
    PICC_CMD_SEL_CL2      = 0x95,
    PICC_CMD_SEL_CL3      = 0x97,
    PICC_CMD_HLTA         = 0x50,
    PICC_CMD_RATS         = 0xE0,
    PICC_CMD_MF_AUTH_KEY_A = 0x60,
    PICC_CMD_MF_AUTH_KEY_B = 0x61,
    PICC_CMD_MF_READ      = 0x30,
    PICC_CMD_MF_WRITE     = 0xA0,
    PICC_CMD_MF_DECREMENT = 0xC0,
    PICC_CMD_MF_INCREMENT = 0xC1,
    PICC_CMD_MF_RESTORE   = 0xC2,
    PICC_CMD_MF_TRANSFER  = 0xB0,
    PICC_CMD_UL_WRITE     = 0xA2,
    MFRC522_MIFARE_ACK    = 0x0A,
} piccCmds_t;


// MFRC522's commands for the PCD.
#define PCD_CMD_IDLE                0x00  // NO action; cancels current command execution
#define PCD_CMD_MEM                 0x01  // Stores 25 bytes into the internal buffer
#define PCD_CMD_GEN_RANDOM_ID       0x02  // Generates a 10-byte random ID number
#define PCD_CMD_CALC_CRC            0x03  // Activates the CRC coprocessor or performs a self-test
#define PCD_CMD_TRANSMIT            0x04  // Transmits data from the FIFO buffer
#define PCD_CMD_NO_CMD_CHANGE       0x07  // Can be used to modify the CommandReg register bits without affecting the command, if any, currently being executed
#define PCD_CMD_RECEIVE             0x08  // Activates the receiver circuits
#define PCD_CMD_TRANSCEIVE          0x0C  // Transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define PCD_CMD_MF_AUTHENT          0x0E  // Performs the MIFARE standard authentication as a reader
#define PCD_CMD_SOFT_RESET          0x0F  // Resets the MFRC522
/** @} */

/**
 * @brief Writes a value to the specified register in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param registerAddress The address of the register to write.
 * @param value The value to write to the register.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_WriteRegister(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t value);

/**
 * @brief Reads the value from the specified register in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param registerAddress The address of the register to read.
 * @param data Pointer to store the read value.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_ReadRegister(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t *data);

/**
 * @brief Reads multiple consecutive registers in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param registerAddress The address of the first register to read.
 * @param dataArr Pointer to the array to store the read values.
 * @param dataSize The number of registers to read.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_ReadRegisterArr(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t *dataArr, uint8_t dataSize);

/**
 * @brief Writes multiple consecutive registers in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param registerAddress The address of the first register to write.
 * @param dataArr Pointer to the array of values to write.
 * @param dataSize The number of registers to write.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_WriteRegisterArr(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t *dataArr, uint8_t dataSize);

/**
 * @brief Initializes the MFRC522 RFID module.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_Init(spi_device_handle_t *spiHandle, uint8_t rstPin);

/**
 * @brief Clears the specified bits in the register of the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param registerAdress The address of the register to modify.
 * @param mask The bitmask of the bits to clear.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_ClrRegBitMask(spi_device_handle_t *spiHandle, uint8_t registerAdress, uint8_t mask);

/**
 * @brief Checks if a card is present.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @return True if a card is present, false otherwise.
 */
bool xMFRC522_IsCardPresent(spi_device_handle_t *spiHandle);

/**
 * @brief Turns on the antenna of the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_AntennaOn(spi_device_handle_t *spiHandle);

/**
 * @brief Performs a self-test of the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_SelfTest(spi_device_handle_t *spiHandle, uint8_t rstPin);

/**
 * @brief Resets the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_Reset(spi_device_handle_t *spiHandle);

/**
 * @brief Transmits data to the MFRC522 and receives the response.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param waitIrq Wait for the command to complete (1) or not (0).
 * @param cmdBuf Pointer to the command buffer to be transmitted.
 * @param bufSize Size of the command buffer.
 * @param bitFrame The bit framing type.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_Transcieve(spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t *cmdBuf, uint8_t bufSize, bitFraming_t bitFrame);

/**
 * @brief Performs MIFARE authentication with the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param waitIrq Wait for the command to complete (1) or not (0).
 * @param cmdBuf Pointer to the command buffer to be transmitted.
 * @param bufSize Size of the command buffer.
 * @param bitFrame The bit framing type.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_MF_Authent(spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t *cmdBuf, uint8_t bufSize, bitFraming_t bitFrame);

/**
 * @brief Performs communication with a MIFARE card using the MFRC522.
 *
 * @param cmd Command to be sent to the MIFARE card.
 * @param spiHandle Pointer to the SPI device handle.
 * @param waitIrq Wait for the command to complete (1) or not (0).
 * @param cmdBuf Pointer to the command buffer to be transmitted.
 * @param bufSize Size of the command buffer.
 * @param bitFrame The bit framing type.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_CommWithMifare(uint8_t cmd, spi_device_handle_t *spiHandle, uint8_t waitIrq, uint8_t *cmdBuf, uint8_t bufSize, bitFraming_t bitFrame);

/**
 * @brief Sets the specified bits in the register of the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param registerAddress The address of the register to modify.
 * @param mask The bitmask of the bits to set.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_SetRegBitMask(spi_device_handle_t *spiHandle, uint8_t registerAddress, uint8_t mask);

/**
 * @brief Calculates the CRC value for the given buffer.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param buf Pointer to the buffer for which the CRC is to be calculated.
 * @param bufLen Length of the buffer.
 * @param resultBuf Pointer to the buffer to store the calculated CRC value (2 bytes).
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMFRC522_CalculateCRC(spi_device_handle_t *spiHandle, uint8_t *buf, uint8_t bufLen, uint8_t resultBuf[2]);

/**
 * @brief Reads the UID from the MFRC522 for the specified UID size.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param uidSize The size of the UID.
 * @return Pointer to the UniqueIdentifier_t structure containing the UID information, or NULL on failure.
 */
UniqueIdentifier_t *xMifare_ReadUID(spi_device_handle_t *spiHandle, uidSize_t uidSize);

/**
 * @brief Retrieves the key data for the specified UID from the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @return Pointer to the Mifare1kKey_t structure containing the key data, or NULL on failure.
 */
Mifare1kKey_t *xMifare_GetKeyData(spi_device_handle_t *spiHandle, UniqueIdentifier_t *UID);

/**
 * @brief Writes the provided key data to the specified UID in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @param data The key data to be written.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMifare_WriteKey(spi_device_handle_t *spiHandle, UniqueIdentifier_t *UID, uint8_t data[45][16]);

/**
 * @brief Writes the provided data to the specified block address for the specified UID in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param blockAddress The block address to write the data to.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @param data The data to be written.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMifare_WriteKeyBlock(spi_device_handle_t *spiHandle, uint8_t blockAddress, UniqueIdentifier_t *UID, uint8_t data[16]);

/**
 * @brief Sets the SAK (Select Acknowledge) byte for the specified UID in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @return ESP_OK if successful, otherwise an error code.
 */
static esp_err_t xMifare_SetSakByte(spi_device_handle_t *spiHandle, UniqueIdentifier_t *UID);

/**
 * @brief Reads the key block at the specified block address for the specified UID in the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param blockAddress The block address to read from.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMifare_ReadKeyBlock(spi_device_handle_t *spiHandle, uint8_t blockAddress, UniqueIdentifier_t *UID);

/**
 * @brief Performs a block check character calculation for the provided data and UID.
 *
 * @param bufData Pointer to the buffer data.
 * @param bufSize Size of the buffer.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @return True if the block check character is valid, false otherwise.
 */
static bool xPrv_Mifare_BlockCheckChar(uint8_t *bufData, uint8_t bufSize, UniqueIdentifier_t *UID);

/**
 * @brief Performs MIFARE authentication for the specified block address and key with the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param cmd The command for the authentication (PICC_CMD_MF_AUTH_KEY_A or PICC_CMD_MF_AUTH_KEY_B).
 * @param blockAddress The block address to authenticate.
 * @param key The key for authentication.
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 * @return ESP_OK if successful, otherwise an error code.
 */
esp_err_t xMifare_Authenticate(spi_device_handle_t *spiHandle, uint8_t cmd, uint8_t blockAddress, uint8_t *key, UniqueIdentifier_t *UID);

/**
 * @brief Prints the UID information to the console.
 *
 * @param UID Pointer to the UniqueIdentifier_t structure containing the UID.
 */
void vMifare_PrintUID(UniqueIdentifier_t *UID);

/**
 * @brief Retrieves and prints the FIFO buffer data from the MFRC522.
 *
 * @param spiHandle Pointer to the SPI device handle.
 * @param fifoBuf Pointer to the buffer to store the FIFO data.
 * @param print Flag indicating whether to print the FIFO data or not.
 */
void vMFRC522_GetAndPrintFifoBuf(spi_device_handle_t *spiHandle, uint8_t *fifoBuf, bool print);

/**
 * @brief Prints the key data to the console.
 *
 * @param key Pointer to the Mifare1kKey_t structure containing the key data.
 */
void vMifare_PrintKey(Mifare1kKey_t *key);


#endif // _MFRC522_H_
