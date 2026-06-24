/**
 * @file    uart_config.h
 * @brief   UART unit configuration enums and structures.
 * @version 1.0
 * @date    2026-04-25
 *
 * Defines the physical UART parameters (pins, baudrate, buffers) for each
 * logical UART channel on the board.
 */

#ifndef UART_CONFIG_H
#define UART_CONFIG_H

#include <stdint.h>
#include "pin_config.h"
#include "driver/uart.h"

/**
 * @brief Enumeration for logical UART channel identifiers.
 */
typedef enum {
    UART_CH_PC = 0,    /**< UART channel for PC communication (typically UART0) */
    UART_CH_1 = 1,     /**< First auxiliary UART channel */
    UART_CH_2 = 2,     /**< Second auxiliary UART channel */
    UART_CH_COUNT      /**< Total count of logical UART channels */
} uart_id_t;

/**
 * @brief Configuration structure for a specific UART unit.
 * Contains all hardware parameters required to initialize and manage
 * an ESP-IDF UART driver instance.
 */
typedef struct {
    uart_port_t port_num;      /**< Hardware UART port number (e.g., UART_NUM_0) */
    uint32_t baudrate;         /**< Communication speed in bits per second */
    int tx_pin;                /**< Physical TX pin number */
    int rx_pin;                /**< Physical RX pin number */
    uint16_t buffer_size;      /**< Size of the internal RX/TX ring buffers in bytes */
    uint32_t ticks_wait;       /**< Maximum FreeRTOS ticks to wait for UART operations */
} uart_unit_cfg_t;

/**
 * @brief Global table of UART unit configurations defined in uart_config.c.
 */
extern const uart_unit_cfg_t g_uart_unit_cfg[UART_CH_COUNT];

#endif // UART_CONFIG_H
