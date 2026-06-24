/**
 * @file    uart_port.h
 * @brief   Hardware Abstraction Layer for UART communication on ESP32.
 * @version 1.0
 * @date    2026-04-25
 *
 * Provides low-level UART port management including open, read, write, and close operations.
 */

#ifndef UART_PORT_H
#define UART_PORT_H

#include "uart_config.h"
#include <stdint.h>

/**
 * @brief Open and prepare a UART port.
 * @param cfg Pointer to UART configuration struct.
 */
void hal_uart_open(const uart_unit_cfg_t *cfg);

/**
 * @brief Write data to the configured UART port.
 * @param cfg Pointer to UART configuration struct.
 * @param data Pointer to data buffer to transmit.
 * @param len Number of bytes to transmit.
 * @return Number of bytes actually written, or -1 on error.
 */
int hal_uart_write(const uart_unit_cfg_t *cfg, const uint8_t *data, uint16_t len);

/**
 * @brief Read data from the configured UART port.
 * @param cfg Pointer to UART configuration struct.
 * @param[out] data Pointer to buffer to store received data.
 * @param len Maximum number of bytes to receive.
 * @return Number of bytes actually read, or -1 on error.
 */
int hal_uart_read(const uart_unit_cfg_t *cfg, uint8_t *data, uint16_t len);

/**
 * @brief Close the UART port and release resources.
 * @param cfg Pointer to UART configuration struct.
 */
void hal_uart_close(const uart_unit_cfg_t *cfg);

#endif // UART_PORT_H
