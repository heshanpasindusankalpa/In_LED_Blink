#ifndef SVC_BUTTON_H
#define SVC_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include "btn_config.h"

typedef enum {
    BTN_IDLE = 0,
    BTN_INSTANT_PRESS,
    BTN_SHORT_PRESS,
    BTN_LONG_PRESS
} button_state_t;

/**
 * @brief Initialize the button service.
 */
void svc_button_init(void);

/**
 * @brief Tick function for the button service.
 */
void svc_button_tick(void);

/**
 * @brief Reads the button event state. Call this periodically.
 * @param btn_id The logical button ID to check.
 * @return The current button state event.
 */
button_state_t svc_button_event_read(btn_index_t btn_id);

#endif /* SVC_BUTTON_H */
