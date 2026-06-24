/* Board-specific YS-S201 (YF-S201) flow sensor configuration */

#include "YS-S201_config.h"

const ys_cfg_t g_ys_cfg[YS_S201_COUNT] = {
    [0] = { .pin_id = GPIO_9 },
};
