/**
 * @file    i2c.c
 * @brief   Hardware abstraction layer for I2C operations.
 * @version 1.0
 * @date    2026-04-27
 */

#include "i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "i2c_port.h"
#include <stdio.h>

#define SLAVE_REG_COUNT 256
static uint8_t s_slave_regs[I2C_CFG_COUNT][SLAVE_REG_COUNT] = {0};
static uint8_t s_active_reg_addr[I2C_CFG_COUNT] = {0};

//-------------------------------------------------------------------------
// SLAVE
//! CHECKED CHE
// Internal task that handles I2C slave transactions and register mapping.
//-------------------------------------------------------------------------
static void i2c_slave_task(void *arg) {
  i2c_index_t bus_id = (i2c_index_t)(uintptr_t)arg;

  printf("I2C Slave Task started for bus %d\n", bus_id);

  while (1) {
    uint8_t rx_buf[2] = {0xCC, 0xCC};

    int rx_len = hal_i2c_slave_read(&g_i2c_cfg[bus_id], rx_buf, 2, 1000);

    if (rx_len > 0) {
      uint8_t reg = rx_buf[0];
      s_active_reg_addr[bus_id] = reg;

      if (rx_len == 2) {
        uint8_t val = rx_buf[1];
        s_slave_regs[bus_id][reg] = val;
        printf("\n---> SUCCESS! Master WROTE to Register 0x%02X: Value = %d <+++\n\n", reg, val);
      } 
      else if (rx_len == 1) {
        uint32_t len_to_send = 1;

        int err = hal_i2c_slave_write(
            &g_i2c_cfg[bus_id], &s_slave_regs[bus_id][reg], len_to_send, 1000);

        printf("Master prepared to READ Register 0x%02X\n", reg);

        if (err != 0) {
          printf("Slave Write Error: %d\n", err);
        }
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

//-------------------------------------------------------------------------
// BOTH
//! CHECKED CHE
// Initializes the I2C peripheral (Master or Slave based on config).
//-------------------------------------------------------------------------
void i2c_init(i2c_index_t bus_id) {
  if (bus_id >= I2C_CFG_COUNT) {
    return;
  }
  hal_i2c_init(&g_i2c_cfg[bus_id]);
}

//-------------------------------------------------------------------------
// BOTH
//! CHECKED CHE
// Deinitializes the I2C peripheral and frees resources.
//-------------------------------------------------------------------------
void i2c_deinit(i2c_index_t bus_id) {
  if (bus_id >= I2C_CFG_COUNT) {
    return;
  }
  hal_i2c_deinit(&g_i2c_cfg[bus_id]);
}

//-------------------------------------------------------------------------
// MASTER
//! CHECKED CHE
// Used by a Master to write a value to a specific register on a Slave device.
//-------------------------------------------------------------------------
int i2c_write_register(i2c_index_t bus_id, uint8_t dev_addr, uint8_t reg_addr,
                       uint8_t value) {
  if (bus_id >= I2C_CFG_COUNT) {
    return -1;
  }
  return hal_i2c_write_register(&g_i2c_cfg[bus_id], dev_addr, reg_addr, value);
}

//-------------------------------------------------------------------------
// MASTER
//! CHECKED CHE
// Used by a Master to read data from a specific register on a Slave device.
//-------------------------------------------------------------------------
int i2c_read_register(i2c_index_t bus_id, uint8_t dev_addr, uint8_t reg_addr,
                      uint8_t *data, size_t len) {
  if (bus_id >= I2C_CFG_COUNT) {
    return -1;
  }
  return hal_i2c_read_register(&g_i2c_cfg[bus_id], dev_addr, reg_addr, data,
                               len);
}

//-------------------------------------------------------------------------
// SLAVE
//! CHECKED CHE
// Starts the background task that manages Slave operations.
//-------------------------------------------------------------------------
void i2c_slave_start(i2c_index_t bus_id) {
  if (bus_id >= I2C_CFG_COUNT)
    return;

  // If RAM access is enabled, prime some example registers
  if (g_i2c_cfg[bus_id].access_ram_en) {
    uint8_t device_id = 0x42; // Example Device ID at Reg 0x00
    uint8_t version = 0x01;   // Example Version at Reg 0x01
    i2c_slave_ram_write(bus_id, 0x00, &device_id, 1);
    i2c_slave_ram_write(bus_id, 0x01, &version, 1);
  }

  xTaskCreate(i2c_slave_task, "i2c_slave_task", 4096, (void *)(uintptr_t)bus_id,
              20, NULL);
}

//-------------------------------------------------------------------------
// SLAVE
//! CHECKED CHE
// Sets a value in the virtual register map of the local Slave.
//-------------------------------------------------------------------------
void i2c_slave_set_variable(i2c_index_t bus_id, uint8_t reg_addr,
                            uint8_t value) {
  if (bus_id >= I2C_CFG_COUNT)
    return;
  s_slave_regs[bus_id][reg_addr] = value;

  // Automatically sync with hardware I2C RAM if enabled
  if (g_i2c_cfg[bus_id].access_ram_en) {
    i2c_slave_ram_write(bus_id, reg_addr, &value, 1);
  }
}

//-------------------------------------------------------------------------
// SLAVE
//! CHECKED CHE
// Retrieves a value from the virtual register map of the local Slave.
//-------------------------------------------------------------------------
uint8_t i2c_slave_get_variable(i2c_index_t bus_id, uint8_t reg_addr) {
  if (bus_id >= I2C_CFG_COUNT)
    return 0;
  return s_slave_regs[bus_id][reg_addr];
}

//-------------------------------------------------------------------------
// SLAVE
//! CHECKED CHE
// Read bytes from I2C internal ram.
//-------------------------------------------------------------------------
int i2c_slave_ram_read(i2c_index_t bus_id, uint8_t ram_address, uint8_t *data, size_t receive_size) {
  if (bus_id >= I2C_CFG_COUNT) {
    return -1;
  }
  return hal_i2c_slave_ram_read(&g_i2c_cfg[bus_id], ram_address, data, receive_size);
}

//-------------------------------------------------------------------------
// SLAVE
//! CHECKED CHE
// Write bytes to I2C internal ram.
//-------------------------------------------------------------------------
int i2c_slave_ram_write(i2c_index_t bus_id, uint8_t ram_address, const uint8_t *data, size_t size) {
  if (bus_id >= I2C_CFG_COUNT) {
    return -1;
  }
  return hal_i2c_slave_ram_write(&g_i2c_cfg[bus_id], ram_address, data, size);
}



