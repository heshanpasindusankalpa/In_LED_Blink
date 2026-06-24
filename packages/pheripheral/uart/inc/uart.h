#ifndef PERIPHERAL_UART_H
#define PERIPHERAL_UART_H

#include <stdint.h>
#include "uart_config.h"
#include "uart_proto.h"

/**
 * @brief Initialize a UART peripheral instance mapped to a logical id.
 */
void uart_init(uart_id_t uart_id);

/**
 * @brief Write data to the given logical UART id using the protocol format.
 * 
 * @param uart_id Logical UART ID.
 * @param cmd_type Protocol command type.
 * @param data Pointer to the payload data.
 * @param len Length of the payload data.
 * @return Number of bytes written or -1 on error.
 */
int uart_write(uart_id_t uart_id, uint8_t cmd_type, const uint8_t *data, uint16_t len);

/**
 * @brief Read data from the given logical UART id and parse using the protocol.
 * 
 * @param uart_id Logical UART ID.
 * @param msg Pointer to the message structure to store parsed data.
 * @return Length of the parsed frame or -1 on error.
 */
int uart_read(uart_id_t uart_id, uart_proto_msg_t *msg);

/**
 * @brief Deinitialize a UART peripheral instance mapped to a logical id.
 */
void uart_deinit(uart_id_t uart_id);

#endif // PERIPHERAL_UART_H
