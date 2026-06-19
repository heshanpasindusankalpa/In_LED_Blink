#ifndef LMP91000_CONFIG_H
#define LMP91000_CONFIG_H

/**
 * @file    lmp91000_config.h
 * @brief   LMP91000 configuration table and enums for board abstraction.
 * @version 1.0
 * @date    2026-04-27
 */

#include "i2c_config.h"
#include "gpio_config.h"
#include <stdint.h>

/**
 * @brief Enum defining all configured LMP91000 sensors.
 */
typedef enum {
  LMP91000_0 = 0U,
  LMP91000_COUNT
} lmp91000_index_t;

/**
 * @brief Configuration structure for LMP91000 instances.
 */
typedef struct {
  i2c_index_t i2c_bus;     /**< I2C bus index */
  uint8_t i2c_address;     /**< Device I2C address */
  gpio_index_t menb_pin;   /**< Logical GPIO index for MENB pin */
} lmp91000_cfg_t;

/**
 * @brief Board-level LMP91000 configuration table.
 */
extern const lmp91000_cfg_t g_lmp91000_cfg[LMP91000_COUNT];

#endif /* LMP91000_CONFIG_H */
