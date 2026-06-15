#include "svc_button.h"
#include "btn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BTN_PRESS_DEBOUNCE_MS   20
#define BTN_SHORT_PRESS_MS      100
#define BTN_LONG_PRESS_MS       1000

typedef struct {
    uint8_t  btn_raw_state;
    uint8_t  debounced_state;
    uint8_t  prev_debounced;
    uint8_t  last_raw;
    uint32_t debounce_start;  
    uint32_t press_start_time;  
    bool     long_press_sent;
} svc_btn_context_t;

static svc_btn_context_t g_btn_ctx[BTN_COUNT];

// checked_CHE
void svc_button_init(void)
{
    for (int i = 0; i < BTN_COUNT; i++) {
        g_btn_ctx[i].btn_raw_state    = 0;
        g_btn_ctx[i].debounced_state  = 0;
        g_btn_ctx[i].prev_debounced   = 0;
        g_btn_ctx[i].last_raw         = 0;
        g_btn_ctx[i].debounce_start   = 0;
        g_btn_ctx[i].press_start_time = 0;
        g_btn_ctx[i].long_press_sent  = false;
    }
}

void svc_button_tick(void)
{
    // Tick processing is handled dynamically inside svc_button_event_read
}

// checked_CHE
button_state_t svc_button_event_read(btn_index_t btn_id)
{
    if (btn_id >= BTN_COUNT) {
        return BTN_IDLE;
    }

    svc_btn_context_t *ctx = &g_btn_ctx[btn_id];
    uint32_t tick_counter = pdTICKS_TO_MS(xTaskGetTickCount());
    
    // Read the physical state of the button
    ctx->btn_raw_state = btn_is_pressed(btn_id) ? 1 : 0;

    if (ctx->btn_raw_state != ctx->last_raw) {
        ctx->last_raw       = ctx->btn_raw_state;
        ctx->debounce_start = tick_counter;
    }

    if ((tick_counter - ctx->debounce_start >= BTN_PRESS_DEBOUNCE_MS) &&
        (ctx->btn_raw_state != ctx->debounced_state)) {

        ctx->debounced_state = ctx->btn_raw_state;

        if (ctx->debounced_state == 1 && ctx->prev_debounced == 0) {
            ctx->prev_debounced   = 1;
            ctx->press_start_time = tick_counter;
            ctx->long_press_sent  = false;
            return BTN_INSTANT_PRESS;
        }
        else if (ctx->debounced_state == 0 && ctx->prev_debounced == 1) {
            ctx->prev_debounced = 0;
            if (!ctx->long_press_sent) {
                return BTN_SHORT_PRESS;
            }
        }
    }
    
    if (ctx->debounced_state == 1 && !ctx->long_press_sent &&
        (tick_counter - ctx->press_start_time >= BTN_LONG_PRESS_MS)) {
        ctx->long_press_sent = true;
        return BTN_LONG_PRESS;
    }

    return BTN_IDLE;
}
