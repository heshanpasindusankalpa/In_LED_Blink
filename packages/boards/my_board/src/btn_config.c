#include "btn_config.h"

const btn_gpio_cfg_t g_btn_gpio_cfg[BTN_COUNT] = {
    [BTN_1] = {
        .btn_id = BTN_1,
        .pin_id = GPIO_2
    },
    [BTN_2] = {
        .btn_id = BTN_2,
        .pin_id = GPIO_3
    },
};


