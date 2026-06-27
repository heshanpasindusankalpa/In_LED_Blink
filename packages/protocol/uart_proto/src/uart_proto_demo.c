/*
 * uart_proto_demo.c
 * Demo helper that builds two sensor frames and transmits them via
 * a platform-provided transmit hook. Intended for quick testing from main().
 */

#include "uart_proto.h"
#include "svc_uart_tx.h"
#include <stdint.h>


/* Build and transmit two example sensor frames. */
void uart_proto_demo_send(void)
{
    uint8_t frame[16];

    float flow1 = 12.34f;
    uint16_t raw1 = (uint16_t)(flow1 * 100.0f + 0.5f);
    uint16_t len = uart_send_id(frame, 1, raw1);   /* sensor ID 1 */
    if (len != 0U) {
        uart_proto_platform_tx(frame, len, 100U);
    }

    float flow2 = 45.67f;
    uint16_t raw2 = (uint16_t)(flow2 * 100.0f + 0.5f);
    len = uart_send_id(frame, 2, raw2);            /* sensor ID 2 */
    if (len != 0U) {
        uart_proto_platform_tx(frame, len, 100U);
    }
}
