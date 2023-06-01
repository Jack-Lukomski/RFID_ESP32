/****************************************************************************
* File Name: ESP 32 BLE Driver
* Author: Jaime Malone
* File Description:
*
*
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"

/**
 * @brief Handles the write event environment.
 *
 * This function is called to handle the write event environment. It takes the GATT server interface,
 * the prepare write environment, and the GATT server callback parameters as input.
 *
 * @param gatts_if GATT server interface.
 * @param prepare_write_env Prepare write environment.
 * @param param GATT server callback parameters.
 */
void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Handles the execute write event environment.
 *
 * This function is called to handle the execute write event environment. It takes the prepare write environment
 * and the GATT server callback parameters as input.
 *
 * @param prepare_write_env Prepare write environment.
 * @param param GATT server callback parameters.
 */
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Handles the GAP event handler.
 *
 * This function is the event handler for the GAP events. It takes the GAP event and the GAP callback parameters as input.
 *
 * @param event GAP event.
 * @param param GAP callback parameters.
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/**
 * @brief Handles the write event environment.
 *
 * This function is called to handle the write event environment. It takes the GATT server interface,
 * the prepare write environment, and the GATT server callback parameters as input.
 *
 * @param gatts_if GATT server interface.
 * @param prepare_write_env Prepare write environment.
 * @param param GATT server callback parameters.
 */
void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Handles the execute write event environment.
 *
 * This function is called to handle the execute write event environment. It takes the prepare write environment
 * and the GATT server callback parameters as input.
 *
 * @param prepare_write_env Prepare write environment.
 * @param param GATT server callback parameters.
 */
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Handles the GATT profile A event handler.
 *
 * This function is the event handler for the GATT profile A events. It takes the GATT event, the GATT server interface,
 * and the GATT server callback parameters as input.
 *
 * @param event GATT event.
 * @param gatts_if GATT server interface.
 * @param param GATT server callback parameters.
 */
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Handles the GATT event handler.
 *
 * This function is the event handler for the GATT events. It takes the GATT event, the GATT server interface,
 * and the GATT server callback parameters as input.
 *
 * @param event GATT event.
 * @param gatts_if GATT server interface.
 * @param param GATT server callback parameters.
 */
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Initializes the BLE.
 *
 * This function initializes the BLE stack and other necessary components.
 */
void BLE_init(void);