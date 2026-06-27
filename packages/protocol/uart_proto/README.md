# UART Protocol Package

This package implements a small framed UART protocol used by the `helloworld` application.
It provides helpers for building and parsing protocol frames, computing checksums, and transmitting sample sensor data frames.

## Package Structure

- `inc/uart_proto.h`
  - public protocol definitions and API declarations.
- `src/uart_proto.c`
  - implementation of frame construction, parsing, checksum calculation, and helper extraction functions.
- `src/uart_proto_demo.c`
  - demo transmit hook and example send routine used by `helloworld`.

## Protocol Overview

The UART protocol frame format is:

```
[0]   START_OF_FRAME (0xAA)
[1]   TYPE
[2]   PAYLOAD_LEN
[3]   PAYLOAD[0]
...   PAYLOAD[n-1]
[n+3] CHECKSUM
```

- `START_OF_FRAME` is always `0xAA` (`UART_PROTO_SOF`).
- `TYPE` identifies the command or payload type.
- `PAYLOAD_LEN` is the number of payload bytes.
- `CHECKSUM` is a simple XOR over `TYPE` and all payload bytes.

## Definitions

- `UART_PROTO_SOF` = `0xAA`
- `UART_PROTO_MAX_PAYLOAD` = `64`
- `CMD_SENSOR_DATA` = `0x01`

## API Reference

### `uint16_t build_frame(uint8_t *buf, uint8_t start_byte, uint8_t type, uint8_t payload_len, const uint8_t *payload, uint16_t checksum);`

Builds a protocol frame into `buf`.

- `buf` must be a valid output buffer.
- `start_byte` is typically `UART_PROTO_SOF`.
- `type` is the protocol command type.
- `payload_len` is number of payload bytes.
- `payload` points to the payload data.
- `checksum` parameter is ignored; the implementation computes the checksum internally.

Returns the total frame length: `4 + payload_len`.

### `uint16_t uart_send_id(uint8_t *buf, uint8_t sensor_id, uint16_t raw);`

Builds a sensor data frame with a fixed 3-byte payload:

- `payload[0] = sensor_id`
- `payload[1] = raw low byte`
- `payload[2] = raw high byte`

`raw` is expected to be a scaled sensor value (for example, `value * 100`).

Returns the generated frame length.

### `uint16_t uart_receive(const uint8_t *buf, uint16_t len, uart_proto_msg_t *msg);`

Parses an incoming receive buffer into `msg`.

Validation performed:

- `buf` and `msg` are non-NULL
- `len >= 4`
- `buf[0] == UART_PROTO_SOF`
- `payload_len <= UART_PROTO_MAX_PAYLOAD`
- `len` contains the full frame
- checksum matches

Returns the parsed frame length on success, or `0` on failure.

### `uint8_t uart_proto_get_sensor_data(const uart_proto_msg_t *msg, uint8_t *sensor_id, uint16_t *raw);`

Extracts sensor metadata from a parsed message.

- Requires `msg->payload_len == 3`
- Returns `1` on success, `0` on failure

### `void uart_proto_demo_send(void);`

Builds two example sensor frames and transmits them by calling `uart_proto_platform_tx()`.
This is useful for verifying that the protocol and transport path are wired correctly.

## Message Format Example

For `sensor_id = 1` and `raw = 1234`:

- frame bytes: `AA 01 03 01 D2 04 <checksum>`
- checksum is `0x01 ^ 0x01 ^ 0xD2 ^ 0x04`

## Platform Integration

The package expects a platform-specific transmit hook:

```c
void uart_proto_platform_tx(const uint8_t *buf, uint16_t len, uint32_t timeout);
```

Implement this function in your board or application layer to forward `buf` to the UART hardware.

Example:

```c
void uart_proto_platform_tx(const uint8_t *buf, uint16_t len, uint32_t timeout)
{
    (void)timeout;
    if (buf == NULL || len == 0U) {
        return;
    }
    hal_uart_write(&g_uart_unit_cfg[UART_CH_PC], buf, len);
}
```

## Terminal Display Notes

The default demo transmits raw protocol frames as binary data.
A normal terminal emulator such as PuTTY or Tera Term will show unreadable characters for this binary stream.

If you want readable output in a terminal emulator, convert the frame bytes to ASCII hex before transmission.

Example conversion output:

```
TX: AA 01 03 01 D2 04 06
```

## Usage Example

In `main()`:

```c
uart_init(UART_CH_PC);
for (;;) {
    uart_proto_demo_send();
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## Notes

- The checksum is intentionally simple for this demo protocol.
- The frame builder ignores the passed `checksum` argument and recalculates it internally.
- The protocol is designed for short sensor-data payloads, not large streaming payloads.
