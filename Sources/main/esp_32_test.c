#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_13
#define DELAY_MS 1000

void app_main() {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);

        gpio_set_level(LED_PIN, 0);
        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
}
