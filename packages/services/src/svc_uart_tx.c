/**
 * @file    svc_uart_tx.c
 * @brief   UART transmit service implementation.
 *
 * Provides the strong definition of uart_proto_platform_tx() that sends
 * framed protocol bytes over UART0 (UART_CH_PC) to the PC terminal.
 *
 * This overrides the weak no-op stub declared in uart_proto_demo.c so the
 * protocol layer can remain board-agnostic while actual transmission is
 * handled here at the service layer.
 */

#include "svc_uart_tx.h"
#include "uart_port.h"     /* HAL layer — hal_uart_write() */
#include "uart_config.h"   /* g_uart_unit_cfg[], UART_CH_PC  */

/* Override the weak demo transmit hook and send raw bytes over UART0. */
void uart_proto_platform_tx(const uint8_t *buf, uint16_t len, uint32_t timeout)
{
    (void)timeout;
    if (buf == NULL || len == 0U) {
        return;
    }

    hal_uart_write(&g_uart_unit_cfg[UART_CH_PC], buf, len);
}
