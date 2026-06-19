#ifndef I2C_PORT_H
#define I2C_PORT_H

/**
 * @file    i2c_port.h
 * @brief   Hardware abstraction layer for ESP32 I2C control.
 * @version 1.0
 * @date    2026-04-27
 */

#include "i2c_config.h"
#include <stdint.h>
#include <stddef.h>
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

/**
 * @brief Initialize the hardware I2C port.
 */
void hal_i2c_init(const i2c_cfg_t *cfg);

/**
 * @brief Deinitialize the hardware I2C port.
 */
void hal_i2c_deinit(const i2c_cfg_t *cfg);

/**
 * @brief Write a value to a register over I2C.
 */
int hal_i2c_write_register(const i2c_cfg_t *cfg, uint8_t dev_addr, uint8_t reg_addr, uint8_t value);

/**
 * @brief Read data from a register over I2C.
 */
int hal_i2c_read_register(const i2c_cfg_t *cfg, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

/**
 * @brief Type for I2C Slave RX callback.
 * Return true if a higher priority task was woken.
 */
typedef bool (*hal_i2c_slave_rx_cb_t)(const uint8_t *buffer, uint32_t len, void *arg);

/**
 * @brief Type for I2C Slave TX request callback.
 * Return true if a higher priority task was woken.
 */
typedef bool (*hal_i2c_slave_tx_cb_t)(void *arg);

typedef struct {
    hal_i2c_slave_rx_cb_t on_rx;
    hal_i2c_slave_tx_cb_t on_tx;
    void *arg;
} hal_i2c_slave_cb_t;

/**
 * @brief Register I2C Slave event callbacks.
 */
int hal_i2c_slave_register_callbacks(const i2c_cfg_t *cfg, const hal_i2c_slave_cb_t *cbs);

/**
 * @brief Write data to I2C Slave FIFO (to be read by Master).
 */
int hal_i2c_slave_write(const i2c_cfg_t *cfg, const uint8_t *data, uint32_t size, uint32_t timeout_ms);

/**
 * @brief Get the pointer to the slave RX buffer
 */
const uint8_t* hal_i2c_slave_get_rx_buf(const i2c_cfg_t *cfg);

/**
 * @brief Read data from I2C Slave FIFO (written by Master).
 */
int hal_i2c_slave_read(const i2c_cfg_t *cfg, uint8_t *data, uint32_t size, uint32_t timeout_ms);

/**
 * @brief Read bytes from I2C internal ram.
 */
int hal_i2c_slave_ram_read(const i2c_cfg_t *cfg, uint8_t ram_address, uint8_t *data, size_t receive_size);

/**
 * @brief Write bytes to I2C internal ram.
 */
int hal_i2c_slave_ram_write(const i2c_cfg_t *cfg, uint8_t ram_address, const uint8_t *data, size_t size);

#endif /* I2C_PORT_H */
