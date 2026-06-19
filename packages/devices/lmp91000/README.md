# LMP91000 Device Driver

A device-level driver for the LMP91000 Analog Front End (AFE) for chemical sensing. This module provides a hardware-independent API for configuring the TIA, bias, and operating modes while decoupling application logic from I2C and GPIO pin assignments.

## Architecture Overview

1.  **Application Layer**: Requests sensor configuration and state changes (e.g., `lmp91000_set_three_lead()`).
2.  **Device Layer (This Folder)**: 
    *   Standardizes LMP91000 operations like TIA gain setting, bias control, and mode selection.
    *   Handles register-level I2C communication and `MENB` (Manual Enable) pin toggling.
3.  **Peripheral Layer (`packages/pheripheral/i2c` & `packages/pheripheral/gpio`)**: Handles the low-level bus communication and pin transitions.
4.  **Board Layer (`packages/boards/my_board`)**: 
    *   **`lmp91000_config.h`**: Defines the logical sensor enumeration (`LMP91000_0`).
    *   **`lmp91000_config.c`**: Maps logical sensor indices to I2C buses and MENB GPIO pins.

---

## How to Use (For Application Developers)

Interact with the LMP91000 using its logical names defined in the board configuration.

```c
#include "lmp91000.h"

void sensor_setup_task(void) {
    // Initialize the hardware mapping and MENB pin
    lmp91000_init(LMP91000_0); 

    // Unlock registers to allow configuration
    lmp91000_unlock(LMP91000_0);

    // Configure TIA gain (e.g., 35k Ohm) and Internal Zero (50%)
    lmp91000_set_gain(LMP91000_0, 3); 
    lmp91000_set_int_z(LMP91000_0, 1);

    // Set to 3-lead amperometric mode
    lmp91000_set_three_lead(LMP91000_0);
    
    // Lock registers to prevent accidental changes
    lmp91000_lock(LMP91000_0);
}
```

---

## How to Configure Hardware (Integrator Guide)

To add a new LMP91000 instance to the board, follow this configuration chain:

### 1. Define the Physical Pin (`pin_config.h`)
Identify the physical MCU pin number assigned to the `MENB` line.
```c
#define PIN_LMP_MENB  4  // Physical MCU pin
```

### 2. Configure the GPIO (`gpio_config.c`)
Add a logical GPIO entry for the MENB pin in the `g_gpio_cfg` table.
```c
[GPIO_LMP_MENB] = {
    .pin_num = PIN_LMP_MENB,
    .direction = GPIO_MODE_OUTPUT,
    .intr_type = GPIO_INTR_DISABLE,
    .pull = GPIO_PULLUP_ONLY,
},
```

### 3. Map to LMP91000 Logic (`lmp91000_config.c`)
Map the logical sensor index to the I2C bus and the configured MENB GPIO index.
```c
[LMP91000_1] = {
    .i2c_bus = I2C_0,
    .i2c_address = 0x48,
    .menb_pin = GPIO_LMP_MENB
},
```

---

## Porting to Another MCU
The LMP91000 driver is hardware-agnostic as long as the underlying `packages/pheripheral/i2c` and `packages/pheripheral/gpio` layers are correctly implemented for the target microcontroller.

---

## API Reference

### 1. Initialization and Control

#### `lmp91000_init`
```c
void lmp91000_init(lmp91000_index_t id);
```
**Description:** Initializes the LMP91000 sensor instances. It sets up the `MENB` GPIO pin and initializes the assigned I2C bus.
- **Parameters:**
  - `id`: The logical sensor identifier (defined in `lmp91000_config.h`).
- **Returns:** None.
- **Notes:** Sets the `MENB` pin high by default to disable communication until needed.

#### `lmp91000_enable`
```c
void lmp91000_enable(lmp91000_index_t id);
```
**Description:** Enables I2C communication by pulling the `MENB` pin low.
- **Parameters:**
  - `id`: Logical sensor index.
- **Returns:** None.
- **Datasheet Note:** `MENB` must be set low for the entire duration of any I2C read or write operation.

#### `lmp91000_disable`
```c
void lmp91000_disable(lmp91000_index_t id);
```
**Description:** Disables I2C communication by setting the `MENB` pin high.
- **Parameters:**
  - `id`: Logical sensor index.
- **Returns:** None.
- **Datasheet Note:** `MENB` should be set high when I2C communication is not active to save power and block the bus.

#### `lmp91000_is_ready`
```c
bool lmp91000_is_ready(lmp91000_index_t id);
```
**Description:** Checks if the device is ready to accept I2C commands by reading the `STATUS` register (0x00).
- **Parameters:**
  - `id`: Logical sensor index.
- **Returns:** `true` if the device is ready (READY bit is 1), `false` otherwise.
- **Datasheet Note:** "The READY bit is set to 1 when the device is ready." Consult Section 7.6.1 of the datasheet.

---

### 2. Register Access and Locking

#### `lmp91000_write_reg`
```c
void lmp91000_write_reg(lmp91000_index_t id, uint8_t reg, uint8_t data);
```
**Description:** Writes a byte of data to a specific device register.
- **Parameters:**
  - `id`: Logical sensor index.
  - `reg`: Target register address.
  - `data`: Byte value to write.
- **Returns:** None.

#### `lmp91000_read_reg`
```c
uint8_t lmp91000_read_reg(lmp91000_index_t id, uint8_t reg);
```
**Description:** Reads a single byte from a specified device register.
- **Parameters:**
  - `id`: Logical sensor index.
  - `reg`: Target register address.
- **Returns:** The byte value read from the register.

#### `lmp91000_lock` / `lmp91000_unlock`
```c
void lmp91000_lock(lmp91000_index_t id);
void lmp91000_unlock(lmp91000_index_t id);
```
**Description:** Controls write access to the `TIACN` (0x10) and `REFCN` (0x11) configuration registers.
- **Parameters:**
  - `id`: Logical sensor index.
- **Returns:** None.
- **Datasheet Note:** "LOCK = 1 (default): TIACN and REFCN are read-only. LOCK = 0: they are writable."

#### `lmp91000_is_locked`
```c
bool lmp91000_is_locked(lmp91000_index_t id);
```
**Description:** Checks if the configuration registers are currently locked.
- **Returns:** `true` if locked, `false` if writable.

---

### 3. TIA and Load Configuration

#### `lmp91000_set_gain`
```c
void lmp91000_set_gain(lmp91000_index_t id, uint8_t gain_value);
```
**Description:** Configures the TIA gain (bits 4:2 of `TIACN`).
- **Parameters:**
  - `gain_value`: Selects the transimpedance amplifier gain.
    - **0**: External resistor
    - **1**: 2.75 kΩ
    - **2**: 3.5 kΩ
    - **3**: 7 kΩ
    - **4**: 14 kΩ
    - **5**: 35 kΩ
    - **6**: 120 kΩ
    - **7**: 350 kΩ
- **Returns:** None.
- **Notes:** Automatically unlocks the register before writing and preserves other bits.

#### `lmp91000_get_gain`
```c
double lmp91000_get_gain(lmp91000_index_t id);
```
**Description:** Returns the current gain setting in Ohms based on the last `set_gain` call.
- **Returns:** Floating point value (e.g., 35000.0).

#### `lmp91000_set_rload`
```c
void lmp91000_set_rload(lmp91000_index_t id, uint8_t load);
```
**Description:** Sets the internal load resistor (bits 1:0 of `TIACN`).
- **Parameters:**
  - `load`: Selects the RLOAD value.
    - **0**: 10 Ω
    - **1**: 33 Ω
    - **2**: 50 Ω
    - **3**: 100 Ω (default)
- **Returns:** None.

#### `lmp91000_set_int_z`
```c
void lmp91000_set_int_z(lmp91000_index_t id, uint8_t intZ);
```
**Description:** Configures the internal zero selection (bits 6:5 of `REFCN`).
- **Parameters:**
  - `intZ`: Selects the internal zero voltage.
    - **0**: 20% of Vref
    - **1**: 50% of Vref
    - **2**: 67% of Vref
    - **3**: Bypassed
- **Returns:** None.

---

### 4. Voltage Reference and Bias

#### `lmp91000_set_ref_source`
```c
void lmp91000_set_ref_source(lmp91000_index_t id, uint8_t source);
```
**Description:** Selects the voltage reference source.
- **Parameters:**
  - `source`:
    - **0**: Internal reference (Vdd)
    - **1**: External reference (Vref pin)

#### `lmp91000_set_bias_sign`
```c
void lmp91000_set_bias_sign(lmp91000_index_t id, uint8_t sign);
```
**Description:** Sets the bias polarity (bit 4 of `REFCN`).
- **Parameters:**
  - `sign`:
    - **0**: Negative bias (V_WE - V_RE < 0V, default)
    - **1**: Positive bias (V_WE - V_RE > 0V)

#### `lmp91000_set_bias`
```c
void lmp91000_set_bias(lmp91000_index_t id, uint8_t bias);
```
**Description:** Sets the bias magnitude as a percentage of Vref (bits 3:0 of `REFCN`).
- **Parameters:**
  - `bias`: Magnitude (0 to 13).
    - **0**: 0% (default)
    - **1**: 1%
    - ...
    - **13**: 24%
    - *Note: Values above 13 are not recommended and may be clamped.*

#### `lmp91000_set_bias_with_sign`
```c
void lmp91000_set_bias_with_sign(lmp91000_index_t id, uint8_t bias, int8_t sign);
```
**Description:** Simultaneously sets both bias magnitude and polarity.
- **Parameters:**
  - `sign`: 0 (Negative), 1 (Positive).
  - `bias`: 0-13 (clamped to 0 if higher).

---

### 5. Operating Modes and Features

#### `lmp91000_set_mode`
```c
void lmp91000_set_mode(lmp91000_index_t id, uint8_t mode);
```
**Description:** Dispatcher for setting the operating mode (bits 2:0 of `MODECN`).
- **Parameters:**
  - `mode`:
    - **0**: Deep Sleep (0.6 μA typical)
    - **1**: 2-lead ground referred galvanic cell
    - **2**: Standby
    - **3**: 3-lead amperometric cell
    - **4**: Temperature measurement (TIA OFF)
    - **5**: Temperature measurement (TIA ON)

#### `lmp91000_set_sleep`
```c
void lmp91000_set_sleep(lmp91000_index_t id);
```
**Description:** Puts the device into Deep Sleep mode.
- **Datasheet Note:** Device typically consumes 0.6 μA in this mode.

#### `lmp91000_set_two_lead`
```c
void lmp91000_set_two_lead(lmp91000_index_t id);
```
**Description:** Sets the LMP91000 to 2-lead ground referred galvanic cell mode (OP_MODE = 001).

#### `lmp91000_standby`
```c
void lmp91000_standby(lmp91000_index_t id);
```
**Description:** Puts the device into Standby mode (OP_MODE = 010).

#### `lmp91000_set_three_lead`
```c
void lmp91000_set_three_lead(lmp91000_index_t id);
```
**Description:** Configures the device for 3-lead amperometric cell mode (OP_MODE = 011).

#### `lmp91000_measure_cell`
```c
void lmp91000_measure_cell(lmp91000_index_t id);
```
**Description:** Sets the LMP91000 to temperature measurement mode with TIA OFF (OP_MODE = 110).

#### `lmp91000_get_temp`
```c
void lmp91000_get_temp(lmp91000_index_t id);
```
**Description:** Sets the LMP91000 to temperature measurement mode with TIA ON (OP_MODE = 111).

#### `lmp91000_set_fet` / `lmp91000_enable_fet` / `lmp91000_disable_fet`
```c
void lmp91000_set_fet(lmp91000_index_t id, uint8_t selection);
void lmp91000_enable_fet(lmp91000_index_t id);
void lmp91000_disable_fet(lmp91000_index_t id);
```
**Description:** Controls the FET shorting feature (bit 7 of `MODECN`).
- **Notes:** Enabling (1) shorts the FET. This is typically used to protect the sensor during transport or power-down.
- **Datasheet Note:** Section 7.6.5, bit 7. 0 = Disabled (default), 1 = Enabled.

---

## Technical Notes

### Temperature Measurement
When the device is in temperature measurement mode (`lmp91000_get_temp` or `lmp91000_measure_cell`), the internal temperature sensor provides a voltage at the `VOUT` pin. Use the following constants (defined in `lmp91000.h`) for conversion:
- **`TEMP_INTERCEPT`**: 1555.0 mV
- **`TEMPSLOPE`**: -8.0 mV/°C

### Configuration Locking
The `TIACN` (Gain/Load) and `REFCN` (Reference/Bias) registers are **locked (read-only) by default** upon power-up. 
- You **must** call `lmp91000_unlock()` before attempting to change gain, bias, or reference settings.
- It is a best practice to call `lmp91000_lock()` after configuration is complete to prevent accidental register writes.

### Hardware Communication (MENB)
The `MENB` (Manual Enable) pin is active-low. The driver automatically manages this pin during register access:
1. Pulls `MENB` low.
2. Performs I2C transaction.
3. Pulls `MENB` high.
*Note: If multiple sensors are on the same I2C bus, ensure each has a unique `MENB` pin assigned in the board configuration.*

### Datasheet References
For detailed timing diagrams and electrical characteristics, refer to the [TI LMP91000 Datasheet](https://www.ti.com/lit/ds/symlink/lmp91000.pdf):
- **Section 7.6.1**: Status Register (Address 0x00)
- **Section 7.6.3**: TIA Control Register (Address 0x10)
- **Section 7.6.4**: Reference Control Register (Address 0x11)
- **Section 7.6.5**: Mode Control Register (Address 0x12)
