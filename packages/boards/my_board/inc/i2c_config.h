#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

/**
 * @file    i2c_config.h
 * @brief   I2C configuration table and enums for board abstraction.
 * @version 1.0
 * @date    2026-04-27
 */

#include "pin_config.h"
#include <stdint.h>
#include "driver/i2c_master.h"


#include "hal/i2c_types.h"

/**
 * @brief Enum defining all configured I2C buses available in the system.
 */
typedef enum {
  I2C_0 = 0U,    /**< Primary I2C bus (Master) */
  I2C_1 = 1U,    /**< Secondary I2C bus (Slave) */
  I2C_CFG_COUNT  /**< Total count of configured I2C buses */
} i2c_index_t;

/**
 * @brief Configuration structure for an I2C device.
 */
typedef struct {
    uint8_t bus_port;
    uint8_t dev_addr;
    i2c_master_dev_handle_t handle;
} i2c_device_cache_t;


/**
 * @brief I2C configuration structure for each logical bus.
 */
typedef struct {
  uint32_t port_num;   /**< Physical I2C port number */
  uint32_t sda_pin;    /**< SDA pin mapped */
  uint32_t scl_pin;    /**< SCL pin mapped */
  uint32_t clk_speed;  /**< I2C clock speed in Hz */
  i2c_mode_t mode;     /**< Master or Slave mode */
  uint8_t slave_addr;  /**< 7-bit Slave address (if mode == I2C_MODE_SLAVE) */
  uint8_t access_ram_en; /**< Enable direct access to I2C RAM (Slave mode only) */
  uint32_t send_buf_depth; /**< Size of the internal transfer buffer (Slave mode) */
  uint8_t stretch_en;      /**< Enable clock stretching (Slave mode) */
  uint8_t broadcast_en;    /**< Enable broadcast addressing (Slave mode) */
  uint8_t slave_unmatch_en; /**< Enable unmatch interrupt (Slave mode) */
  int intr_priority;        /**< I2C interrupt priority (0 for default) */
} i2c_cfg_t;

/**
 * @brief Board-level I2C configuration table.
 */
extern const i2c_cfg_t g_i2c_cfg[I2C_CFG_COUNT];

#endif /* I2C_CONFIG_H */
