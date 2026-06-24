/**
 * @file    uart_config.c
 * @brief   Implementation of UART configuration tables for the board.
 * @version 1.0
 * @date    2026-04-25
 */

#include "uart_config.h"

#define UART_CFG_BAUD_RATE 115200
#define UART_CFG_RX_BUFFER_SIZE 1024
#define UART_CFG_WAIT_TICKS 20U

/**
 * @brief Board-level configuration table for all UART units.
 */
const uart_unit_cfg_t g_uart_unit_cfg[UART_CH_COUNT] = {
    [UART_CH_PC] = {
        .port_num = UART_NUM_0,
        .baudrate = UART_CFG_BAUD_RATE,
        .tx_pin = UART_MASTER_TX0,
        .rx_pin = UART_MASTER_RX0,
        .buffer_size = UART_CFG_RX_BUFFER_SIZE,
        .ticks_wait = UART_CFG_WAIT_TICKS,
    },
    [UART_CH_1] = {
        .port_num = UART_NUM_1,
        .baudrate = UART_CFG_BAUD_RATE,
        .tx_pin = UART_MASTER_TX1,
        .rx_pin = UART_MASTER_RX1,
        .buffer_size = UART_CFG_RX_BUFFER_SIZE,
        .ticks_wait = UART_CFG_WAIT_TICKS,
    },
    [UART_CH_2] = {
        .port_num = UART_NUM_2,
        .baudrate = UART_CFG_BAUD_RATE,
        .tx_pin = UART_SLAVE_TX,
        .rx_pin = UART_SLAVE_RX,
        .buffer_size = UART_CFG_RX_BUFFER_SIZE,
        .ticks_wait = UART_CFG_WAIT_TICKS,
    },
};
