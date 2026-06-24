/**
 * @file    uart.c
 * @brief   Application-level UART wrappers that manage frame building and parsing.
 * @version 1.0
 * @date    2026-04-25
 *
 * Provides simplified UART access by bridging higher-level protocols to the hardware abstraction layer.
 */

#include "uart.h"
#include "uart_port.h"

/**
 * @brief Initialize a UART channel using pre-configured unit settings.
 */
void uart_init(uart_id_t uart_id)
{
    if (uart_id >= UART_CH_COUNT) {
        return;
    }
    hal_uart_open(&g_uart_unit_cfg[uart_id]);
}

/**
 * @brief Build a protocol frame and send it over UART.
 */
int uart_write(uart_id_t uart_id, uint8_t cmd_type, const uint8_t *data, uint16_t len)
{
    if (uart_id >= UART_CH_COUNT || len > UART_PROTO_MAX_PAYLOAD) {
        return -1;
    }

    uint8_t tx_buffer[UART_PROTO_MAX_PAYLOAD + 4U];
    uint16_t frame_len = build_frame(tx_buffer, UART_PROTO_SOF, cmd_type, (uint8_t)len, data, 0U);

    if (frame_len == 0U) {
        return -1;
    }

    return hal_uart_write(&g_uart_unit_cfg[uart_id], tx_buffer, frame_len);
}

/**
 * @brief Read and parse a protocol frame from UART.
 */
int uart_read(uart_id_t uart_id, uart_proto_msg_t *msg)
{ 
    if (uart_id >= UART_CH_COUNT || msg == NULL) {
        return -1;
    }

    uint8_t rx_buffer[UART_PROTO_MAX_PAYLOAD + 4U];
    int rx_bytes = hal_uart_read(&g_uart_unit_cfg[uart_id], rx_buffer, (uint16_t)sizeof(rx_buffer));

    if (rx_bytes <= 0) {
        return -1;
    }

    uint16_t parsed_len = uart_receive(rx_buffer, (uint16_t)rx_bytes, msg);
    if (parsed_len == 0U) {
        return -1;
    }

    return (int)parsed_len;
}

/**
 * @brief Close a UART channel and release associated hardware resources.
 */
void uart_deinit(uart_id_t uart_id)
{
    if (uart_id >= UART_CH_COUNT) {
        return;
    }
    hal_uart_close(&g_uart_unit_cfg[uart_id]);
}
