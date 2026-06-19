/* Minimal BQ27427 driver (device copy) */

#include "bq27427_config.h"
#include "i2c.h"
#include <stdint.h>
#include "gpio.h"

void bq27427_init(bq27427_index_t id)
{
    if (id >= BQ27427_COUNT) {
        return;
    }
    i2c_init(g_bq27427_cfg[id].i2c_bus);
}

void bq27427_write_word(bq27427_index_t id, uint8_t reg, uint16_t data)
{
    if (id >= BQ27427_COUNT) {
        return;
    }
    uint8_t lsb = (uint8_t)(data & 0xFFu);
    uint8_t msb = (uint8_t)((data >> 8) & 0xFFu);
    (void)i2c_write_register(g_bq27427_cfg[id].i2c_bus, g_bq27427_cfg[id].i2c_address, reg, lsb);
    (void)i2c_write_register(g_bq27427_cfg[id].i2c_bus, g_bq27427_cfg[id].i2c_address, reg + 1U, msb);
}

uint16_t bq27427_read_word(bq27427_index_t id, uint8_t reg)
{
    if (id >= BQ27427_COUNT) {
        return 0U;
    }
    uint8_t buf[2] = {0, 0};
    int rc = i2c_read_register(g_bq27427_cfg[id].i2c_bus, g_bq27427_cfg[id].i2c_address, reg, buf, 2U);
    if (rc < 0) {
        return 0U;
    }
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

void bq27427_write_byte(bq27427_index_t id, uint8_t reg, uint8_t data)
{
    if (id >= BQ27427_COUNT) {
        return;
    }
    (void)i2c_write_register(g_bq27427_cfg[id].i2c_bus, g_bq27427_cfg[id].i2c_address, reg, data);
}

uint8_t bq27427_read_byte(bq27427_index_t id, uint8_t reg)
{
    if (id >= BQ27427_COUNT) {
        return 0U;
    }
    uint8_t value = 0U;
    int rc = i2c_read_register(g_bq27427_cfg[id].i2c_bus, g_bq27427_cfg[id].i2c_address, reg, &value, 1U);
    if (rc < 0) {
        return 0U;
    }
    return value;
}

