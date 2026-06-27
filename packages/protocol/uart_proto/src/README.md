# README — `uart_proto.c`

> **File:** `uart_proto.c`
> **Location:** `helloworld/packages/protocol/uart_proto/src/`
> **Brief:** Simple UART protocol framing and parsing helpers for a sweat-sensor protocol layer.

---

## Overview

This file implements three layers of the UART frame protocol:

| Concern | Function |
|---|---|
| Checksum computation | `uart_proto_checksum()` (static/internal) |
| Frame construction | `build_frame()` |
| Sensor-data shortcut | `uart_send_id()` |
| Frame parsing | `uart_receive()` |
| Payload extraction | `uart_proto_get_sensor_data()` |

---

## Line-by-Line Summary

### File Header (Lines 1–9)

| Line(s) | Description |
|---|---|
| 1 | Opens the Doxygen block-comment for the file header. |
| 2 | `@file` tag — declares the file name as `uart_proto.c`. |
| 3 | `@brief` tag — one-line description: "Simple UART protocol framing and parsing helpers." |
| 4 | `@version` tag — marks the file at version `1`. |
| 5 | `@date` tag — records the creation date `2026-06-25`. |
| 6 | Blank comment line (separator). |
| 7 | Extended description: states the file implements frame construction, checksum calculation, and basic parsing. |
| 8 | Continues the extended description: specifies use by "the sweat sensor protocol layer." |
| 9 | Closes the Doxygen block-comment. |

---

### Includes (Lines 11–13)

| Line(s) | Description |
|---|---|
| 11 | `#include <stdint.h>` — pulls in fixed-width integer types (`uint8_t`, `uint16_t`, etc.) from the C standard library. |
| 12 | `#include "stddef.h"` — includes `stddef.h` (locally quoted form) for `NULL` and `size_t`. |
| 13 | `#include "uart_proto.h"` — includes the module's own header, which declares public types (`uart_proto_msg_t`), constants (`UART_PROTO_SOF`, `CMD_SENSOR_DATA`, `UART_PROTO_MAX_PAYLOAD`), and function prototypes. |

---

### `uart_proto_checksum()` — XOR Checksum (Lines 15–29)

```c
static uint8_t uart_proto_checksum(uint8_t type, const uint8_t *payload, uint16_t payload_len)
```

| Line(s) | Description |
|---|---|
| 15–17 | Doxygen comment: describes the function as computing a simple XOR checksum for a frame payload. |
| 18 | Function signature — `static` (file-private), returns `uint8_t`. Parameters: `type` (frame type byte), `payload` (pointer to payload bytes), `payload_len` (number of payload bytes). |
| 19 | Opens the function body `{`. |
| 20 | Declares loop index `i` as `uint16_t`. |
| 21 | Initialises `checksum` with `type` — the checksum seeds itself from the command/type byte. |
| 22 | Blank line. |
| 23 | `for` loop — iterates from `0` to `payload_len - 1`. |
| 24 | Opens loop body `{`. |
| 25 | XORs each payload byte into `checksum` (`checksum ^= payload[i]`). Cast to `uint8_t` suppresses implicit-promotion warnings. |
| 26 | Closes loop body `}`. |
| 27 | Blank line. |
| 28 | Returns the computed checksum byte. |
| 29 | Closes function body `}`. |

---

### `build_frame()` — Frame Construction (Lines 31–60)

```c
uint16_t build_frame(uint8_t *buf, uint8_t start_byte, uint8_t type,
                     uint8_t payload_len, const uint8_t *payload, uint16_t checksum)
```

**Frame layout produced:**

```
[ start_byte | type | payload_len | payload[0..N-1] | checksum ]
```

| Line(s) | Description |
|---|---|
| 31–33 | Doxygen comment: describes building a framed UART packet into `buf`. |
| 34 | Function signature — public, returns `uint16_t` (total frame length in bytes). Parameters: output buffer `buf`, `start_byte` (SOF marker), `type` (command), `payload_len`, `payload` data, and a `checksum` parameter (unused — see line 37). |
| 35 | Opens function body `{`. |
| 36 | Declares loop index `i` as `uint16_t`. |
| 37 | `(void)checksum;` — explicitly silences the "unused parameter" compiler warning; the checksum is computed internally via `uart_proto_checksum()` rather than being passed in. |
| 38 | Blank line. |
| 39–42 | Guard 1: if `buf` is `NULL`, returns `0` immediately (no output buffer available). |
| 43 | Blank line. |
| 44–47 | Guard 2: if `payload_len > 0` but `payload` is `NULL`, returns `0` (would dereference a null pointer). |
| 48 | Blank line. |
| 49 | Writes the start-of-frame byte into `buf[0]`. |
| 50 | Writes the command/type byte into `buf[1]`. |
| 51 | Writes `payload_len` (cast to `uint8_t`) into `buf[2]`. |
| 52 | Blank line. |
| 53–56 | Loop copies each payload byte into `buf[3 + i]`. |
| 57 | Blank line. |
| 58 | Calls `uart_proto_checksum()` and appends its result to `buf[3 + payload_len]` (byte immediately after the payload). |
| 59 | Returns total frame size: `4 + payload_len` (1 SOF + 1 type + 1 length + N payload + 1 checksum). |
| 60 | Closes function body `}`. |

---

### `uart_send_id()` — Sensor Data Frame Helper (Lines 62–82)

```c
uint16_t uart_send_id(uint8_t *buf, uint8_t sensor_id, uint16_t raw)
```

**Payload layout:**

| Byte | Content |
|---|---|
| `payload[0]` | `sensor_id` |
| `payload[1]` | Low byte of `raw` |
| `payload[2]` | High byte of `raw` |

| Line(s) | Description |
|---|---|
| 62–72 | Doxygen comment: describes this as a convenience helper for building a sensor data frame. Documents the 3-byte payload layout and notes that `raw` is expected to be pre-scaled (e.g., float x 100) by the caller. |
| 73 | Function signature — public, returns `uint16_t` (frame length). Parameters: `buf` (output buffer), `sensor_id` (which sensor), `raw` (scaled 16-bit sensor value). |
| 74 | Opens function body `{`. |
| 75 | Declares a local 3-byte `payload` array on the stack. |
| 76 | Blank line. |
| 77 | Sets `payload[0]` to `sensor_id`. |
| 78 | Sets `payload[1]` to the low byte of `raw` (masked with `0xFF`). |
| 79 | Sets `payload[2]` to the high byte of `raw` (shifted right 8 bits, masked with `0xFF`). Little-endian byte order. |
| 80 | Blank line. |
| 81 | Calls `build_frame()` with: SOF = `UART_PROTO_SOF`, type = `CMD_SENSOR_DATA`, length = `3`, the local payload, and a dummy checksum of `0` (ignored by `build_frame`). Returns the result directly. |
| 82 | Closes function body `}`. |

---

### `uart_receive()` — Frame Parser (Lines 84–128)

```c
uint16_t uart_receive(const uint8_t *buf, uint16_t len, uart_proto_msg_t *msg)
```

| Line(s) | Description |
|---|---|
| 84–86 | Doxygen comment: describes parsing a received UART buffer into a `uart_proto_msg_t` structure. |
| 87 | Function signature — public, returns `uint16_t` (bytes consumed, or `0` on error). Parameters: `buf` (raw received bytes), `len` (number of bytes available), `msg` (output structure). |
| 88 | Opens function body `{`. |
| 89–92 | Guard 1: returns `0` if `buf` or `msg` is `NULL`, or the buffer has fewer than 4 bytes (minimum frame size). |
| 93 | Blank line. |
| 94–97 | Guard 2: returns `0` if `buf[0]` is not `UART_PROTO_SOF` (start-of-frame mismatch). |
| 98 | Blank line. |
| 99 | Reads the command/type byte from `buf[1]` into local variable `type`. |
| 100 | Reads the declared payload length from `buf[2]` into local variable `payload_len`. |
| 101–104 | Guard 3: returns `0` if `payload_len` exceeds `UART_PROTO_MAX_PAYLOAD` (prevents buffer overrun). |
| 105 | Blank line. |
| 106–109 | Guard 4: returns `0` if total buffer `len` is shorter than `4 + payload_len` (incomplete frame). |
| 110 | Blank line. |
| 111 | Reads the received checksum byte from `buf[3 + payload_len]`. |
| 112–115 | Guard 5: recomputes the expected checksum and returns `0` if it does not match (data integrity failure). |
| 116 | Blank line. |
| 117 | Stores `buf[0]` (SOF) into `msg->start_byte`. |
| 118 | Stores `type` into `msg->type`. |
| 119 | Stores `payload_len` into `msg->payload_len`. |
| 120 | Blank line. |
| 121–124 | Loop copies `payload_len` bytes from `buf[3..]` into `msg->payload[]`. |
| 125 | Blank line. |
| 126 | Stores the validated checksum into `msg->checksum`. |
| 127 | Returns `4 + payload_len` — the number of bytes consumed from the input buffer. |
| 128 | Closes function body `}`. |

---

### `uart_proto_get_sensor_data()` — Payload Extractor (Lines 130–151)

```c
uint8_t uart_proto_get_sensor_data(const uart_proto_msg_t *msg,
                                   uint8_t *sensor_id, uint16_t *raw)
```

| Line(s) | Description |
|---|---|
| 130–134 | Doxygen comment: describes extracting `sensor_id` and the raw scaled value from a parsed message. `@return` documents `1` on success, `0` if payload size is wrong. |
| 135 | Function signature — public, returns `uint8_t` (`1` = success, `0` = error). Parameters: `msg` (parsed message), `sensor_id` (output), `raw` (output). |
| 136 | Opens function body `{`. |
| 137–140 | Guard 1: returns `0` if any of the three pointers (`msg`, `sensor_id`, `raw`) is `NULL`. |
| 141 | Blank line. |
| 142–145 | Guard 2: returns `0` if `msg->payload_len` is not exactly `3` (wrong message type or malformed data). |
| 146 | Blank line. |
| 147 | Reads `payload[0]` into `*sensor_id`. |
| 148 | Reconstructs the 16-bit `raw` value from little-endian bytes: `payload[1]` is the low byte OR'd with `payload[2]` shifted left by 8. |
| 149 | Blank line. |
| 150 | Returns `1U` to indicate successful extraction. |
| 151 | Closes function body `}`. |

---

## Frame Format Reference

```
+------------+--------+-------------+-----------------------+----------+
| Byte 0     | Byte 1 | Byte 2      | Bytes 3 to 3+N-1     | Byte 3+N |
+------------+--------+-------------+-----------------------+----------+
| start_byte | type   | payload_len | payload[0 to N-1]    | checksum |
| (SOF)      |        | (N)         |                       | (XOR)    |
+------------+--------+-------------+-----------------------+----------+
```

- **Minimum frame size:** 4 bytes (SOF + type + length + checksum, zero-length payload).
- **Checksum algorithm:** XOR of `type` byte seeded with all payload bytes.
- **Byte order:** Little-endian for multi-byte values (e.g., `raw` sensor data).

---

## Key Constants (defined in `uart_proto.h`)

| Constant | Purpose |
|---|---|
| `UART_PROTO_SOF` | Start-of-frame marker byte |
| `CMD_SENSOR_DATA` | Command type for sensor data frames |
| `UART_PROTO_MAX_PAYLOAD` | Maximum allowed payload length (guards against buffer overruns) |

---

## Dependencies

| Header | Purpose |
|---|---|
| `<stdint.h>` | Fixed-width integer types (`uint8_t`, `uint16_t`) |
| `"stddef.h"` | `NULL` macro, `size_t` |
| `"uart_proto.h"` | Module types, constants, and public function prototypes |
