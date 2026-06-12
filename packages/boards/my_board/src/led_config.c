#include "led_config.h"

const led_cfg_t g_led_gpio_cfg[LED_COUNT] = {
    [LED_1] = { .pin_id = GPIO_0 },[LED_2] = { .pin_id = GPIO_1 },
};
