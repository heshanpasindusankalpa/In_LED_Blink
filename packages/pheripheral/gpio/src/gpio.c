#include "../inc/gpio.h"
#include <stdint.h>

/* forward declarations to avoid including board-specific headers here */
void hal_gpio_set_direction(const gpio_cfg_t *cfg);
void hal_gpio_set_level(const gpio_cfg_t *cfg, uint8_t level);

void gpio_init(gpio_index_t pin_id) {
  if (pin_id >= GPIO_CFG_COUNT) {
    return;
  }
  hal_gpio_set_direction(&g_gpio_cfg[pin_id]);
}

void gpio_set_high(gpio_index_t pin_id) {
  if (pin_id >= GPIO_CFG_COUNT) {
    return;
  }
  hal_gpio_set_level(&g_gpio_cfg[pin_id], 1);
}

void gpio_set_low(gpio_index_t pin_id) {
  if (pin_id >= GPIO_CFG_COUNT) {
    return;
  }
  hal_gpio_set_level(&g_gpio_cfg[pin_id], 0);
}

void gpio_read(gpio_index_t pin_id, uint8_t *level) {
  if (pin_id >= GPIO_CFG_COUNT || level == NULL) {
    return;
  }
  hal_gpio_read(&g_gpio_cfg[pin_id], level);
}
void gpio_isr_service_install(int flags) {
  hal_gpio_install_isr_service(flags);
}

void gpio_register_isr(gpio_index_t pin_id, app_gpio_isr_t isr_handler,
                       void *args) {
  if (pin_id >= GPIO_CFG_COUNT) {
    return;
  }
  hal_gpio_isr_handler_add(&g_gpio_cfg[pin_id], isr_handler, args);
}


void gpio_config_interrupt(gpio_index_t pin_id) {
  if (pin_id >= GPIO_CFG_COUNT) {
    return;
  }
  hal_gpio_config_interrupt(&g_gpio_cfg[pin_id]);
}

void gpio_set_pull(gpio_index_t pin_id) {
  if (pin_id >= GPIO_CFG_COUNT) {
    return;
  }
  hal_gpio_set_pull(&g_gpio_cfg[pin_id]);
}

