#ifndef LED_H
#define LED_H

#include "led_config.h"
#include "gpio.h"


void led_init(led_gpio_index_t led_id);

void led_on(led_gpio_index_t led_id);

void led_off(led_gpio_index_t led_id);

#endif /* LED_H */
