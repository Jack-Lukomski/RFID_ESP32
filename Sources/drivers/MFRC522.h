#ifndef _MFRC522_H_
#define _MFRC522_H_

#include "driver/spi_master.h"
#include "esp_timer.h"

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

// Commands sent to the PICC.
#define PICC_CMD_REQA               0x26  // Request command A
#define PICC_CMD_WUPA               0x52  // Wake-UP command A
#define PICC_CMD_CT                 0x88  // Cascade Tag
#define PICC_CMD_SEL_CL1            0x93  // Select Card Level 1
#define PICC_CMD_SEL_CL2            0x95  // Select Card Level 2
#define PICC_CMD_SEL_CL3            0x97  // Select Card Level 3
#define PICC_CMD_HLTA               0x50  // Halt command A
#define PICC_CMD_AUTH_KEY_A         0x60  // Authentication Key A
#define PICC_CMD_AUTH_KEY_B         0x61  // Authentication Key B

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


esp_err_t MFRC522_WriteRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t value);
esp_err_t MFRC522_ReadRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t * data);
esp_err_t MFRC522_Init(spi_device_handle_t * spiHandle);
void MFRC522_AntennaOn(spi_device_handle_t * spiHandle);

#endif // _MFRC522_H_
