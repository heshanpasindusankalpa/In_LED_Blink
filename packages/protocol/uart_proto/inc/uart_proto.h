/*
 * uart_proto.h
 * Public declarations for the simple UART framing protocol used by the
 * peripheral UART package.
 */

#ifndef UART_PROTO_H
#define UART_PROTO_H

#include <stdint.h>
#include <stddef.h>

/* Protocol definitions */
#define UART_PROTO_SOF            0xAAU
#define UART_PROTO_MAX_PAYLOAD    64U

/* Command IDs */
#define CMD_SENSOR_DATA           0x01U

typedef struct {
	uint8_t start_byte;
	uint8_t type;
	uint8_t payload_len;
	uint8_t payload[UART_PROTO_MAX_PAYLOAD];
	uint8_t checksum;
} uart_proto_msg_t;

/* Frame helpers */
uint16_t build_frame(uint8_t *buf, uint8_t start_byte, uint8_t type, uint8_t payload_len, const uint8_t *payload, uint16_t checksum);
uint16_t uart_send_id(uint8_t *buf, uint8_t sensor_id, uint16_t raw);
uint16_t uart_receive(const uint8_t *buf, uint16_t len, uart_proto_msg_t *msg);
uint8_t uart_proto_get_sensor_data(const uart_proto_msg_t *msg, uint8_t *sensor_id, uint16_t *raw);

/* Demo helper (builds and transmits example frames) */
void uart_proto_demo_send(void);

#endif /* UART_PROTO_H */
