#ifndef YS_S201_CONFIG_H
#define YS_S201_CONFIG_H

#include "gpio_types.h"
#include <stdint.h>

typedef uint8_t ys_index_t;

#ifndef YS_S201_COUNT
#define YS_S201_COUNT 1
#endif

typedef struct {
    gpio_index_t pin_id;
} ys_cfg_t;

extern const ys_cfg_t g_ys_cfg[YS_S201_COUNT];

#endif /* YS_S201_CONFIG_H */
