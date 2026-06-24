/**
 * @file    uart_port.c
 * @brief   Board-level UART HAL implementation for ESP32.
 * @version 1.0
 * @date    2026-04-25
 *
 * Provides low-level UART port management using the ESP-IDF UART driver.
 */

#include "uart_port.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include <stddef.h>
#include "esp_check.h"

/**
 * @brief Open and prepare a UART port.
 */
void hal_uart_open(const uart_unit_cfg_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    uart_config_t uart_config = {
        .baud_rate = cfg->baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(cfg->port_num, cfg->buffer_size * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(cfg->port_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(cfg->port_num, cfg->tx_pin, cfg->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

/**
 * @brief Write data to the configured UART port.
 */
int hal_uart_write(const uart_unit_cfg_t *cfg, const uint8_t *data, uint16_t len)
{
    if (cfg == NULL || data == NULL || len == 0) {
        return -1;
    }

    int txBytes = uart_write_bytes(cfg->port_num, (const char *)data, (size_t)len);
    return txBytes;
}

/**
 * @brief Read data from the configured UART port.
 */
int hal_uart_read(const uart_unit_cfg_t *cfg, uint8_t *data, uint16_t len)
{
    if (cfg == NULL || data == NULL || len == 0) {
        return -1;
    }

    int rxBytes = uart_read_bytes(cfg->port_num, data, (uint32_t)len, (TickType_t)cfg->ticks_wait);
    return rxBytes;
}

/**
 * @brief Close the UART port and release resources.
 */
void hal_uart_close(const uart_unit_cfg_t *cfg)
{
    if (cfg == NULL) {
        return;
    }
    ESP_ERROR_CHECK(uart_driver_delete(cfg->port_num));
}
