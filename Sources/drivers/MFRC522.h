#ifndef MFRC522_H
#define MFRC522_H

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "MRFC522_COM_REGS.h"
#include "MFRC522_CMD_STATUS_REGS.h"
#include "MFRC522_CONFIG_REGS.h"
#include <stdbool.h>

esp_err_t MFRC522_SendCmd (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t cmd);
esp_err_t MFRC522_ReadRegister(spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t *data);
esp_err_t MFRC522_WriteRegister (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t value);
bool CardPresent(spi_device_handle_t *spiHandle);
esp_err_t MFRC522_Init(spi_device_handle_t * spiHandle);
esp_err_t MRFC552_SelfTest(spi_device_handle_t * spiHandle);

#endif