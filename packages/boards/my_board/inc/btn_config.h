
#ifndef BTN_CONFIG_H
#define BTN_CONFIG_H

#include <stdint.h>
#include "gpio_config.h"


typedef enum {
    BTN_1 = 0,   /**< First logical button */
    BTN_2,       /**< Second logical button */
    BTN_3,       /**< Third logical button */
    BTN_COUNT    /**< Total count of logical buttons */
} btn_index_t;

typedef struct {
    btn_index_t btn_id;   /**< Logical button index */
    gpio_index_t pin_id;  /**< Assigned logical GPIO pin index */
} btn_gpio_cfg_t;


extern const btn_gpio_cfg_t g_btn_gpio_cfg[BTN_COUNT];

#endif // BTN_CONFIG_H

