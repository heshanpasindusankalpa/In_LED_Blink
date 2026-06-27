/**
 * @file    svc_uart_tx.h
 * @brief   UART transmit service — platform hook for the uart_proto layer.
 *
 * Provides the strong definition of uart_proto_platform_tx() that wires
 * the portable protocol demo to the board-level HAL (UART_CH_PC / UART0).
 */

#ifndef SVC_UART_TX_H
#define SVC_UART_TX_H

#include <stdint.h>

/**
 * @brief Transmit raw bytes over UART0 (PC channel).
 *
 * Overrides the weak no-op stub in uart_proto_demo.c.
 * Called by uart_proto_demo_send() after building each sensor frame.
 *
 * @param buf     Pointer to the byte buffer to transmit.
 * @param len     Number of bytes to transmit.
 * @param timeout Unused on ESP32 (uart_write_bytes is non-blocking).
 */
void uart_proto_platform_tx(const uint8_t *buf, uint16_t len, uint32_t timeout);

#endif /* SVC_UART_TX_H */
