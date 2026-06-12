#include "led.h"


void led_init(led_gpio_index_t led_id){
    if (led_id >= LED_COUNT) {
        return;
    }
    gpio_init(g_led_gpio_cfg[led_id].pin_id);
}


void led_on(led_gpio_index_t led_id){
    if (led_id >= LED_COUNT) {
        return;
    }
    gpio_set_high(g_led_gpio_cfg[led_id].pin_id);
}

void led_off(led_gpio_index_t led_id){
    if (led_id >= LED_COUNT) {
        return;
    }
    gpio_set_low(g_led_gpio_cfg[led_id].pin_id);
}
