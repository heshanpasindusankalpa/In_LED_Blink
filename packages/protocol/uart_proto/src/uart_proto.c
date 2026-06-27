/**
 * @file    uart_proto.c
 * @brief   Simple UART protocol framing and parsing helpers.
 * @version 1
 * @date    2026-06-25
 *
 * Implements frame construction, checksum calculation, and basic parsing
 * used by the sweat sensor protocol layer.
 */

#include <stdint.h>
#include "stddef.h"
#include "uart_proto.h"

/**
 * @brief Compute simple XOR checksum for a frame payload.
 */
static uint8_t uart_proto_checksum(uint8_t type, const uint8_t *payload, uint16_t payload_len)
{
	uint16_t i;
	uint8_t checksum = type;

	for (i = 0U; i < payload_len; i++)
	{
		checksum = (uint8_t)(checksum ^ payload[i]);
	}

	return checksum;
}

/**
 * @brief Build a framed UART packet into `buf`.
 */
uint16_t build_frame(uint8_t *buf, uint8_t start_byte, uint8_t type, uint8_t payload_len, const uint8_t *payload, uint16_t checksum)
{
	uint16_t i;
	(void)checksum;

	if (buf == NULL)
	{
		return 0U;
	}

	if ((payload_len > 0U) && (payload == NULL))
	{
		return 0U;
	}

	buf[0] = start_byte;
	buf[1] = type;
	buf[2] = (uint8_t)payload_len;

	for (i = 0U; i < payload_len; i++)
	{
		buf[3U + i] = payload[i];
	}

	buf[3U + payload_len] = uart_proto_checksum(type, payload, payload_len);
	return (uint16_t)(4U + payload_len);
}

/**
 * @brief Convenience helper to build a sensor data frame.
 *
 * Payload layout:
 *   payload[0]     = sensor_id
 *   payload[1]     = raw value, low byte
 *   payload[2]     = raw value, high byte
 *
 * `raw` is expected to be the sensor's float value already scaled
 * (e.g. multiplied by 100) and rounded by the caller before this is called.
 */
uint16_t uart_send_id(uint8_t *buf, uint8_t sensor_id, uint16_t raw)
{
	uint8_t payload[3];

	payload[0] = sensor_id;
	payload[1] = (uint8_t)(raw & 0xFFU);
	payload[2] = (uint8_t)((raw >> 8U) & 0xFFU);

	return build_frame(buf, UART_PROTO_SOF, CMD_SENSOR_DATA, 3U, payload, 0U);
}

/**
 * @brief Parse a received UART buffer into a `uart_proto_msg_t` structure.
 */
uint16_t uart_receive(const uint8_t *buf, uint16_t len, uart_proto_msg_t *msg)
{
	if ((buf == NULL) || (msg == NULL) || (len < 4U))
	{
		return 0U;
	}

	if (buf[0] != UART_PROTO_SOF)
	{
		return 0U;
	}

	uint8_t type = buf[1];
	uint8_t payload_len = buf[2];
	if (payload_len > UART_PROTO_MAX_PAYLOAD)
	{
		return 0U;
	}

	if (len < (uint16_t)(4U + payload_len))
	{
		return 0U;
	}

	uint8_t checksum = buf[3U + payload_len];
	if (checksum != uart_proto_checksum(type, &buf[3U], payload_len))
	{
		return 0U;
	}

	msg->start_byte = buf[0];
	msg->type = type;
	msg->payload_len = payload_len;

	for (uint16_t i = 0U; i < payload_len; i++)
	{
		msg->payload[i] = buf[3U + i];
	}

	msg->checksum = checksum;
	return (uint16_t)(4U + payload_len);
}

/**
 * @brief Extract sensor_id and raw value (as scaled uint16) from a parsed message.
 *
 * @return 1 on success, 0 if payload_len doesn't match expected sensor-data size.
 */
uint8_t uart_proto_get_sensor_data(const uart_proto_msg_t *msg, uint8_t *sensor_id, uint16_t *raw)
{
	if ((msg == NULL) || (sensor_id == NULL) || (raw == NULL))
	{
		return 0U;
	}

	if (msg->payload_len != 3U)
	{
		return 0U;
	}

	*sensor_id = msg->payload[0];
	*raw = (uint16_t)(msg->payload[1] | ((uint16_t)msg->payload[2] << 8U));

	return 1U;
}