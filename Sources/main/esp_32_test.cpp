extern "C" {
    #include <stdio.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include <driver/spi_master.h>
    #include <driver/gpio.h>
    #include <time.h>
    #include <string.h>
}

#include "../drivers/MFRC522.hpp"

#define SPI_BUS       HSPI_HOST    // SPI bus to use
#define SPI_CLOCK     (APB_CLK_FREQ/20)      // SPI clock frequency (4 MHz)
#define PIN_MISO      19           // GPIO pin for MISO (Master In Slave Out)
#define PIN_MOSI      18           // GPIO pin for MOSI (Master Out Slave In)
#define PIN_CLK       5           // GPIO pin for clock signal
#define PIN_CS        4           // GPIO pin for chip select (CS)
#define PIN_RST       22   // GPIO pin for reset (RST)

spi_device_handle_t spiHandle;

void setupRFID(spi_device_handle_t spiHandle) 
{
    MFRC522 rfidModule(spiHandle, PIN_RST);

    while(1)
    {
        printf("CPP working!\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

extern "C" void app_main()
{
    spi_bus_config_t busConfig = {
        .mosi_io_num = PIN_MOSI, // or .data0_io_num = PIN_MOSI
        .miso_io_num = PIN_MISO, // or .data1_io_num = PIN_MISO
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,     // or .data2_io_num = -1
        .quadhd_io_num = -1,     // or .data3_io_num = -1
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0,
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
        .flags = 0,
        .queue_size = 1,
        .pre_cb = nullptr,  // replace NULL with nullptr in C++
        .post_cb = nullptr, // replace NULL with nullptr in C++
    };


    spi_bus_initialize(SPI_BUS, &busConfig, 1);
    spi_bus_add_device(SPI_BUS, &devConfig, &spiHandle);

    setupRFID(spiHandle);

    while(1)
    {
        printf("C working!\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
