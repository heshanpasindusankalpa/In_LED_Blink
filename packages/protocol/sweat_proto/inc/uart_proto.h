/**
 * @file    uart_proto.h
 * @brief   Protocol definitions and function prototypes for sweat sensor communication.
 * @version 1.0
 * @date    2026-04-25
 *
 * Defines the frame structure, command types, and provide prototypes for
 * building and parsing UART protocol messages.
 */

#ifndef UART_PROTO_H
#define UART_PROTO_H

#include <stdint.h>

#define UART_PROTO_MAX_PAYLOAD 32U

typedef enum
{
    CMD_UNKNOWN = 0x04U,
    CMD_SENSOR_DATA = 0x05U
} uart_proto_cmd_t;

typedef struct
{
    uint8_t start_byte;
    uint8_t type;
    uint8_t payload_len;
    uint8_t payload[UART_PROTO_MAX_PAYLOAD];
    uint8_t checksum;
} uart_proto_msg_t;

#define UART_PROTO_SOF 0xA5U

/**
 * @brief Build a framed UART packet into `buf`.
 * @param[out] buf Pointer to destination buffer for the frame.
 * @param start_byte Start of Frame (SOF) byte.
 * @param type Command type byte.
 * @param payload_len Length of the payload data.
 * @param payload Pointer to payload data buffer.
 * @param checksum Not used (recalculated internally).
 * @return Length of built frame or 0 on error.
 */
uint16_t build_frame(uint8_t *buf, uint8_t start_byte, uint8_t type, uint8_t payload_len, const uint8_t *payload, uint16_t checksum);

/**
 * @brief Convenience helper to build a sensor data frame.
 * @param[out] buf Pointer to destination buffer for the frame.
 * @param raw Raw 16-bit sensor data value.
 * @return Length of built frame or 0 on error.
 */
uint16_t uart_send(uint8_t *buf, uint16_t raw);

/**
 * @brief Parse a received UART buffer into a `uart_proto_msg_t` structure.
 * @param buf Pointer to buffer containing received frame data.
 * @param len Total number of bytes in the buffer.
 * @param[out] msg Pointer to message struct to store parsed results.
 * @return Length of parsed frame or 0 on error.
 */
uint16_t uart_receive(const uint8_t *buf, uint16_t len, uart_proto_msg_t *msg);

#endif /* UART_PROTO_H */
