#include "gpio_port.h"
#include "../../../pheripheral/gpio/inc/gpio.h"


#include <stdbool.h>
#include "gpio_port.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_check.h"
#include <stdbool.h>

static bool s_gpio_isr_service_installed = false;

void hal_gpio_set_direction(const gpio_cfg_t *cfg) {
  if (cfg == NULL) {
    return;
  }
  gpio_set_direction((gpio_num_t)cfg->pin_num, cfg->direction);
}


void hal_gpio_set_level(const gpio_cfg_t *cfg, uint8_t level) {
  if (cfg == NULL) {
    return;
  }
  gpio_set_level((gpio_num_t)cfg->pin_num, level ? 1 : 0);
}

void hal_gpio_read(const gpio_cfg_t *cfg, uint8_t *level) {
  if (cfg == NULL || level == NULL) {
    return;
  }
  int raw_level = gpio_get_level((gpio_num_t)cfg->pin_num);
  if (raw_level < 0) {
    *level = 0;
  } else {
    *level = (uint8_t)raw_level;
  }
}


static void IRAM_ATTR gpio_isr_dispatcher(void *arg) {
  uint32_t pin_id = (uint32_t)arg;
  if (pin_id < GPIO_CFG_COUNT && g_gpio_isr_table[pin_id].handler) {
    g_gpio_isr_table[pin_id].handler(g_gpio_isr_table[pin_id].args);
  }
}


void hal_gpio_isr_handler_add(const gpio_cfg_t *cfg, app_gpio_isr_t isr_handler,
                              void *args) {
  if (cfg == NULL || isr_handler == NULL) {
    return;
  }
  // Calculate logical index using pointer arithmetic
  uint32_t pin_id = cfg - g_gpio_cfg;

  if (pin_id < GPIO_CFG_COUNT) {
    g_gpio_isr_table[pin_id].handler = isr_handler;
    g_gpio_isr_table[pin_id].args = args;

    // Pass the logical index to the dispatcher
    gpio_isr_handler_add((gpio_num_t)cfg->pin_num, gpio_isr_dispatcher,
                         (void *)pin_id);
  }
}


void hal_gpio_install_isr_service(int flags) {
  if (!s_gpio_isr_service_installed) {
    ESP_ERROR_CHECK(gpio_install_isr_service(flags));
    s_gpio_isr_service_installed = true;
  }
}

void hal_gpio_config_interrupt(const gpio_cfg_t *cfg) {
  if (cfg == NULL) {
    return;
  }
  hal_gpio_install_isr_service(0);
  gpio_set_intr_type((gpio_num_t)cfg->pin_num, cfg->intr_type);
}


void hal_gpio_set_pull(const gpio_cfg_t *cfg) {
  if (cfg == NULL) {
    return;
  }
  gpio_set_pull_mode((gpio_num_t)cfg->pin_num, cfg->pull);
}
