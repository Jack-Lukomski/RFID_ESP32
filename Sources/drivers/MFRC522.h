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

esp_err_t MFRC522_SendCmd (spi_device_handle_t * spiHandle, uint8_t registerAddress, uint8_t cmd);

#endif