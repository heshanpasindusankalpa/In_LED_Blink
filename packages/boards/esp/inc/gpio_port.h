#ifndef GPIO_PORT_H
#define GPIO_PORT_H

#include "driver/gpio.h"
#include <stdint.h>
#include "gpio_config.h"



void hal_gpio_set_direction(const gpio_cfg_t *cfg);
void hal_gpio_set_level(const gpio_cfg_t *cfg, uint8_t level);

void hal_gpio_read(const gpio_cfg_t *cfg, uint8_t *level);


void hal_gpio_install_isr_service(int flags);

void hal_gpio_isr_handler_add(const gpio_cfg_t *cfg, app_gpio_isr_t isr_handler,
                              void *args);

void hal_gpio_config_interrupt(const gpio_cfg_t *cfg);


void hal_gpio_set_pull(const gpio_cfg_t *cfg);

#endif /* GPIO_PORT_H */