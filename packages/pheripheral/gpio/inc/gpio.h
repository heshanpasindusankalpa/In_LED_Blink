#ifndef GPIO_H
#define GPIO_H

#include "gpio_config.h"
#include "gpio_port.h"

void gpio_init(gpio_index_t pin_id);

void gpio_set_high(gpio_index_t pin_id);

void gpio_set_low(gpio_index_t pin_id);

void gpio_read(gpio_index_t pin_id, uint8_t *level);

void gpio_isr_service_install(int flags);

void gpio_register_isr(gpio_index_t pin_id, app_gpio_isr_t isr_handler,
                       void *args);

void gpio_config_interrupt(gpio_index_t pin_id);

void gpio_set_pull(gpio_index_t pin_id);

#endif /* GPIO_H */
