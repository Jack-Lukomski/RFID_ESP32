#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "../drivers/MFRC522.h"
#include <time.h>
#include <string.h>

#define SPI_BUS       HSPI_HOST    // SPI bus to use
#define SPI_CLOCK     100000      // SPI clock frequency (10 MHz)
#define PIN_MISO      19           // GPIO pin for MISO (Master In Slave Out)
#define PIN_MOSI      18           // GPIO pin for MOSI (Master Out Slave In)
#define PIN_CLK       5           // GPIO pin for clock signal
#define PIN_CS        4           // GPIO pin for chip select (CS)
#define PIN_RST       22   // GPIO pin for reset (RST)

spi_device_handle_t spiHandle;

void app_main()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL<<PIN_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // Set the RST pin high to enable the normal operation of the MFRC522
    gpio_set_level(PIN_RST, 1);

    spi_bus_config_t busConfig = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };

    spi_device_interface_config_t devConfig = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = SPI_CLOCK,
        .input_delay_ns = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
        .flags = 0,
    };

    spi_bus_initialize(SPI_BUS, &busConfig, 1);
    spi_bus_add_device(SPI_BUS, &devConfig, &spiHandle);

    while(1)
    {
        MFRC522_Init(&spiHandle);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
