/**
 * @file    i2c_port.c
 * @brief   Board-level I2C HAL for ESP32.
 * @version 1.0
 * @date    2026-04-27
 */

#include "i2c_port.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include <string.h>

#define MAX_I2C_PORTS 2
#define MAX_I2C_DEVICES 10

static i2c_master_bus_handle_t s_bus_handles[MAX_I2C_PORTS] = {NULL};
static i2c_slave_dev_handle_t s_slave_handles[MAX_I2C_PORTS] = {NULL};
static i2c_device_cache_t s_dev_cache[MAX_I2C_DEVICES] = {0};
static int s_dev_count = 0;
static SemaphoreHandle_t s_slave_rx_sem[MAX_I2C_PORTS] = {NULL};

// --- Virtual RAM for Slave Version 2 ---
static uint8_t s_slave_ram[MAX_I2C_PORTS][128] = {0};
static uint8_t s_slave_reg_addr[MAX_I2C_PORTS] = {0};

// Version 2: Data received from Master
static bool i2c_slave_on_receive_cb(i2c_slave_dev_handle_t channel,
                                    const i2c_slave_rx_done_event_data_t *edata,
                                    void *user_data) {
  uint32_t port = (uint32_t)(uintptr_t)user_data;

  if (edata->length > 0) {
    // First byte is usually the register address
    s_slave_reg_addr[port] = edata->buffer[0];

    // Remaining bytes are data to write to our virtual RAM
    for (int i = 1; i < edata->length; i++) {
      uint8_t addr = (s_slave_reg_addr[port] + (i - 1)) % 128;
      s_slave_ram[port][addr] = edata->buffer[i];
    }

    if (s_slave_rx_sem[port]) {
      BaseType_t high_task_wakeup = pdFALSE;
      xSemaphoreGiveFromISR(s_slave_rx_sem[port], &high_task_wakeup);
      return high_task_wakeup == pdTRUE;
    }
  }
  return false;
}

static bool i2c_slave_on_request_cb(i2c_slave_dev_handle_t channel,
                                    const i2c_slave_request_event_data_t *edata,
                                    void *user_data) {
  uint32_t port = (uint32_t)(uintptr_t)user_data;
  uint32_t write_len = 0;

  // Send data from our virtual RAM starting at the last set register address
  i2c_slave_write(channel, &s_slave_ram[port][s_slave_reg_addr[port]], 1,
                  &write_len, 0);

  return false;
}

static i2c_master_dev_handle_t
get_or_add_device(uint8_t port_num, uint8_t dev_addr, uint32_t clk_speed) {
  for (int i = 0; i < s_dev_count; i++) {
    if (s_dev_cache[i].bus_port == port_num &&
        s_dev_cache[i].dev_addr == dev_addr) {
      return s_dev_cache[i].handle;
    }
  }

  if (s_dev_count >= MAX_I2C_DEVICES || port_num >= MAX_I2C_PORTS) {
    return NULL;
  }

  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = dev_addr,
      .scl_speed_hz = clk_speed,
  };

  i2c_master_dev_handle_t dev_handle;
  esp_err_t err =
      i2c_master_bus_add_device(s_bus_handles[port_num], &dev_cfg, &dev_handle);
  if (err != ESP_OK) {
    return NULL;
  }

  s_dev_cache[s_dev_count].bus_port = port_num;
  s_dev_cache[s_dev_count].dev_addr = dev_addr;
  s_dev_cache[s_dev_count].handle = dev_handle;
  s_dev_count++;

  return dev_handle;
}

void hal_i2c_init(const i2c_cfg_t *cfg) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS)
    return;

  if (cfg->mode == I2C_MODE_MASTER) {
    if (s_bus_handles[cfg->port_num] != NULL)
      return; // Already initialized
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = cfg->port_num,
        .sda_io_num = cfg->sda_pin,
        .scl_io_num = cfg->scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&bus_cfg, &s_bus_handles[cfg->port_num]);
  } else {
    if (s_slave_handles[cfg->port_num] != NULL)
      return; // Already initialized

    i2c_slave_config_t slave_cfg = {.i2c_port = cfg->port_num,
                                    .clk_source = I2C_CLK_SRC_DEFAULT,
                                    .scl_io_num = cfg->scl_pin,
                                    .sda_io_num = cfg->sda_pin,
                                    .slave_addr = cfg->slave_addr,
                                    .addr_bit_len = I2C_ADDR_BIT_LEN_7,
                                    .send_buf_depth = cfg->send_buf_depth,
                                    .receive_buf_depth =
                                        256, // New in Version 2
                                    .intr_priority = cfg->intr_priority,
                                    .flags = {
                                        .enable_internal_pullup = true,
                                    }};
    i2c_new_slave_device(&slave_cfg, &s_slave_handles[cfg->port_num]);

    // Force internal pull-ups for Slave pins AFTER driver init
    gpio_set_pull_mode(cfg->sda_pin, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(cfg->scl_pin, GPIO_PULLUP_ONLY);

    if (s_slave_rx_sem[cfg->port_num] == NULL) {
      s_slave_rx_sem[cfg->port_num] = xSemaphoreCreateBinary();
    }

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_on_receive_cb,
        .on_request = i2c_slave_on_request_cb,
    };
    i2c_slave_register_event_callbacks(s_slave_handles[cfg->port_num], &cbs,
                                       (void *)(uintptr_t)cfg->port_num);
  }
}

void hal_i2c_deinit(const i2c_cfg_t *cfg) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS)
    return;

  if (cfg->mode == I2C_MODE_MASTER) {
    // First remove all cached devices for this bus
    for (int i = 0; i < s_dev_count; i++) {
      if (s_dev_cache[i].bus_port == cfg->port_num &&
          s_dev_cache[i].handle != NULL) {
        i2c_master_bus_rm_device(s_dev_cache[i].handle);
        s_dev_cache[i].handle = NULL; // Mark as removed, though cache
                                      // compaction is skipped for simplicity
      }
    }

    if (s_bus_handles[cfg->port_num] != NULL) {
      i2c_del_master_bus(s_bus_handles[cfg->port_num]);
      s_bus_handles[cfg->port_num] = NULL;
    }
  } else {
    if (s_slave_handles[cfg->port_num] != NULL) {
      i2c_del_slave_device(s_slave_handles[cfg->port_num]);
      s_slave_handles[cfg->port_num] = NULL;
    }
  }
}

int hal_i2c_write_register(const i2c_cfg_t *cfg, uint8_t dev_addr,
                           uint8_t reg_addr, uint8_t value) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS ||
      s_bus_handles[cfg->port_num] == NULL) {
    return -1;
  }

  i2c_master_dev_handle_t dev_handle =
      get_or_add_device(cfg->port_num, dev_addr, cfg->clk_speed);
  if (dev_handle == NULL) {
    return -1;
  }

  uint8_t write_buf[2] = {reg_addr, value};
  esp_err_t err = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf),
                                      pdMS_TO_TICKS(1000));
  return (err == ESP_OK) ? 0 : -1;
}

int hal_i2c_read_register(const i2c_cfg_t *cfg, uint8_t dev_addr,
                          uint8_t reg_addr, uint8_t *data, size_t len) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS ||
      s_bus_handles[cfg->port_num] == NULL) {
    return -1;
  }

  i2c_master_dev_handle_t dev_handle =
      get_or_add_device(cfg->port_num, dev_addr, cfg->clk_speed);
  if (dev_handle == NULL) {
    return -1;
  }

  esp_err_t err = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data,
                                              len, pdMS_TO_TICKS(1000));
  return (err == ESP_OK) ? 0 : -1;
}

int hal_i2c_slave_register_callbacks(const i2c_cfg_t *cfg,
                                     const hal_i2c_slave_cb_t *cbs) {
  // In synchronous mode, we don't register callbacks at the hardware level.
  // We will just call i2c_slave_receive/transmit directly in the task.
  return 0;
}

int hal_i2c_slave_write(const i2c_cfg_t *cfg, const uint8_t *data,
                        uint32_t size, uint32_t timeout_ms) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS ||
      s_slave_handles[cfg->port_num] == NULL) {
    return -1;
  }

  uint32_t write_len = 0;
  esp_err_t err = i2c_slave_write(s_slave_handles[cfg->port_num], data, size,
                                  &write_len, pdMS_TO_TICKS(timeout_ms));
  return (err == ESP_OK) ? (int)write_len : -1;
}

int hal_i2c_slave_read(const i2c_cfg_t *cfg, uint8_t *data, uint32_t size,
                       uint32_t timeout_ms) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS ||
      s_slave_handles[cfg->port_num] == NULL) {
    return -1;
  }

  // In Version 2, data is received in the callback and stored in s_slave_ram.
  // We wait for the semaphore which is signaled when data arrives.
  if (xSemaphoreTake(s_slave_rx_sem[cfg->port_num],
                     pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
    // Return the data from our virtual RAM to keep task logic happy
    memcpy(data, &s_slave_ram[cfg->port_num][s_slave_reg_addr[cfg->port_num]],
           size);
    return (int)size;
  }
  return 0; // Timeout
}

int hal_i2c_slave_ram_read(const i2c_cfg_t *cfg, uint8_t ram_address,
                           uint8_t *data, size_t receive_size) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS) {
    return -1;
  }

  // Read from our virtual software RAM
  memcpy(data, &s_slave_ram[cfg->port_num][ram_address], receive_size);
  return 0;
}

int hal_i2c_slave_ram_write(const i2c_cfg_t *cfg, uint8_t ram_address,
                            const uint8_t *data, size_t size) {
  if (cfg == NULL || cfg->port_num >= MAX_I2C_PORTS) {
    return -1;
  }

  // Write to our virtual software RAM
  memcpy(&s_slave_ram[cfg->port_num][ram_address], data, size);
  return 0;
}

const uint8_t *hal_i2c_slave_get_rx_buf(const i2c_cfg_t *cfg) {
  return NULL; // Not used in synchronous mode
}
