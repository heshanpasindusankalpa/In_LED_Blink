/*
 * Board-specific bq27427 configuration
 */

#include "bq27427_config.h"

const bq27427_cfg_t g_bq27427_cfg[BQ27427_COUNT] = {
    [0] = {
        .i2c_bus = I2C_0,
        .i2c_address = 0x55,
    },
};
