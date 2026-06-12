#include "gpio_config.h"
#include "driver/gpio.h"


const gpio_cfg_t g_gpio_cfg[GPIO_CFG_COUNT] = {
    [GPIO_0] = {
        .pin_num = PIN_38,
        .direction = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull = GPIO_PULLUP_ONLY,
    },
    [GPIO_1] = {
        .pin_num = PIN_5,
        .direction = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull = GPIO_PULLUP_ONLY,
    },
    [GPIO_2] = {
        .pin_num = PIN_4,
        .direction = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_NEGEDGE,
        .pull = GPIO_PULLUP_ONLY,
    },
    [GPIO_3] = {
        .pin_num = PIN_18,
        .direction = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
        .pull = GPIO_PULLUP_ONLY,
    },
    [GPIO_4] = {
        .pin_num = PIN_19,
        .direction = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
        .pull = GPIO_PULLUP_ONLY,
    },
};




const size_t g_gpio_cfg_count = GPIO_CFG_COUNT;
app_gpio_isr_state_t g_gpio_isr_table[GPIO_CFG_COUNT] = {0};

