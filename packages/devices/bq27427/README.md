# BQ27427 Device Package

This package provides drivers and examples for the Texas Instruments (BQ) BQ27427 battery fuel gauge used with ESP-IDF projects.

## Contents

- `src/` - driver source files
- `include/` - public headers (if present)
- Examples and wiring notes below

## Features

- Read battery voltage, current, and state-of-charge (SoC)
- Access device registers over I2C

## Hardware connections

- Connect BQ27427 SDA to the ESP32 SDA pin (I2C SDA)
- Connect BQ27427 SCL to the ESP32 SCL pin (I2C SCL)
- Connect VCC to the battery/system supply as required by the gauge
- Connect GND to system ground

Typical I2C address: 0x55 (verify with your module)

## Build / Integration (ESP-IDF)

1. Ensure I2C is enabled and configured in your project (driver installed and pins set).
2. Add this package source to your component path or include it in `components/`.
3. Include the header in your application: `#include "bq27427.h"`

Example C usage (simplified):

```c
// Initialize I2C before calling device functions
esp_err_t err = bq27427_init(I2C_NUM_0, SDA_PIN, SCL_PIN);
if (err != ESP_OK) { /* handle error */ }

uint16_t voltage_mv;
if (bq27427_read_voltage(&voltage_mv) == ESP_OK) {
    printf("Battery: %u mV\n", voltage_mv);
}

uint8_t soc_percent;
if (bq27427_read_soc(&soc_percent) == ESP_OK) {
    printf("State of Charge: %u%%\n", soc_percent);
}
```

Refer to the driver's header for the full API and return values.

## Configuration

- I2C bus selection and pin assignments should match your project's `i2c` configuration.
- Some modules require specific wake/reset sequencing; consult your hardware documentation.

## Troubleshooting

- If the device is not detected, use an I2C scanner to confirm the address.
- Ensure pull-up resistors on SDA/SCL are present if not on the module.

## License

This package inherits the project's license. Check the top-level LICENSE file.

---
If you want, I can add a minimal example application under `examples/` and link to the specific header functions used.
