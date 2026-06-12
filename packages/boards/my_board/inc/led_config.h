// LED mapping for the board
#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include "gpio_types.h"
#include "gpio_config.h"

typedef enum {
	LED_1 = 0U,
	LED_2=1U,
	LED_COUNT
} led_gpio_index_t;

typedef struct {
	gpio_index_t pin_id; /**< Logical GPIO index from gpio_config */
} led_cfg_t;

extern const led_cfg_t g_led_gpio_cfg[LED_COUNT];

#endif /* LED_CONFIG_H */
