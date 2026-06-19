/**
 * @file    lmp91000_config.c
 * @brief   Implementation of LMP91000 configuration table for board abstraction.
 * @version 1.0
 * @date    2026-04-27
 */

#include "lmp91000_config.h"

/**
 * @brief Board-level LMP91000 configuration table.
 */
const lmp91000_cfg_t g_lmp91000_cfg[LMP91000_COUNT] = {
    [LMP91000_0] = {
        .i2c_bus = I2C_0,
        .i2c_address = 0x48,
        .menb_pin = GPIO_0, // Configured for physical pin 4 in gpio_config.c
    },
};
