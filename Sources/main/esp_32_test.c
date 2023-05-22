#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

#define SPI_BUS       HSPI_HOST    // SPI bus to use
#define SPI_CLOCK     10000000      // SPI clock frequency (10 MHz)
#define PIN_MISO      19           // GPIO pin for MISO (Master In Slave Out)
#define PIN_MOSI      18           // GPIO pin for MOSI (Master Out Slave In)
#define PIN_CLK       5           // GPIO pin for clock signal
#define PIN_CS        4           // GPIO pin for chip select (CS)

spi_device_handle_t spiHandle;

void app_main()
{
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

    // Send command
    uint8_t txData[64] = {0};  // Transmit data buffer
    uint8_t rxData[64] = {0};  // Receive data buffer


    spi_transaction_t trans = {
        .tx_buffer = txData,
        .rx_buffer = rxData,
        .length = 8 * 7,  // 8 bits per byte, 7 bytes total
        // Set other transaction parameters (if needed)
    };

    // Command to perform the self-test (0x1F)


    esp_err_t err = spi_device_transmit(spiHandle, &trans);
    
    if (err != ESP_OK) {
        printf("SPI transmit error\n");
        return;
    }
    // Check the response for the self-test result
    uint8_t selfTestResult = rxData[1];  // The self-test result is at index 1

    printf("%x\n", selfTestResult);
    spi_bus_remove_device(spiHandle);
    spi_bus_free(SPI_BUS);
}
