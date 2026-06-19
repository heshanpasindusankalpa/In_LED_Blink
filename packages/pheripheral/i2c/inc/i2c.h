#ifndef I2C_H
#define I2C_H

/**
 * @file    i2c.h
 * @brief   Hardware abstraction layer for I2C operations.
 * @version 1.0
 * @date    2026-04-27
 */

#include "i2c_config.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initializes the hardware I2C port linked to the input enum.
 */
void i2c_init(i2c_index_t bus_id);

/**
 * @brief Deinitializes the hardware I2C port.
 */
void i2c_deinit(i2c_index_t bus_id);

/**
 * @brief Write a single byte to a device register over I2C.
 */
int i2c_write_register(i2c_index_t bus_id, uint8_t dev_addr, uint8_t reg_addr, uint8_t value);

/**
 * @brief Read bytes from a device register over I2C.
 */
int i2c_read_register(i2c_index_t bus_id, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

/**
 * @brief Initialize the I2C slave task and register callbacks.
 */
void i2c_slave_start(i2c_index_t bus_id);

/**
 * @brief Update a variable (register) in the slave's memory.
 * This allows the ESP to update a value that a master can later read.
 */
void i2c_slave_set_variable(i2c_index_t bus_id, uint8_t reg_addr, uint8_t value);

/**
 * @brief Read a variable (register) from the slave's memory.
 * This allows the ESP to read a value that a master wrote to it.
 */
uint8_t i2c_slave_get_variable(i2c_index_t bus_id, uint8_t reg_addr);

/**
 * @brief Read bytes from I2C internal ram (Slave mode).
 */
int i2c_slave_ram_read(i2c_index_t bus_id, uint8_t ram_address, uint8_t *data, size_t receive_size);

/**
 * @brief Write bytes to I2C internal ram (Slave mode).
 */
int i2c_slave_ram_write(i2c_index_t bus_id, uint8_t ram_address, const uint8_t *data, size_t size);

#endif /* I2C_H */
