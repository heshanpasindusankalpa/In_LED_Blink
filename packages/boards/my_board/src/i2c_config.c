/**
 * @file    i2c_config.c
 * @brief   Implementation of I2C configuration table for board abstraction.
 * @version 1.0
 * @date    2026-04-27
 */

#include "i2c_config.h"

/**
 * @brief Board-level I2C configuration table.
 */
const i2c_cfg_t g_i2c_cfg[I2C_CFG_COUNT] = {
    [I2C_0] =
        {
            .port_num = 0,
            .sda_pin = 8,
            .scl_pin = 9,
            .clk_speed = 100000,
            .mode = I2C_MODE_MASTER,
            .slave_addr = 0,
            .access_ram_en = 0,
            .send_buf_depth = 0,
            .stretch_en = 0,
            .broadcast_en = 0,
            .slave_unmatch_en = 0,
            .intr_priority = 0,
        },
    [I2C_1] =
        {
            .port_num = 1,
            .sda_pin = 8,
            .scl_pin = 9,
            .clk_speed = 100000,
            .mode = I2C_MODE_SLAVE,
            .slave_addr = 0x45,
            .access_ram_en = 1,
            .send_buf_depth = 128,
            .stretch_en = 1,
            .broadcast_en = 0,
            .slave_unmatch_en = 1,
            .intr_priority = 3,
        },
};
