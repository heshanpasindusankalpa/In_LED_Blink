#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "led.h"
#include "svc_led.h"

/* Blink timing (ms) */
#define BLINK_ON_MS       120
#define BLINK_OFF_MS      120
#define INTER_BLINK_MS     80
#define LONG_DELAY_MS    1000

typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_SINGLE,
    LED_STATE_DOUBLE,
    LED_STATE_TRIPLE,
} led_state_t;

static volatile led_state_t s_state = LED_STATE_SINGLE;

/* Single-item queue used as a mailbox for button events. Using a length-1
 * queue preserves the previous single-mailbox semantics while making
 * deliveries atomic and safe for ISR/task usage when the correct API
 * is used by senders.
 */
static QueueHandle_t s_svc_led_ev_q = NULL;

void svc_led_set_state(svc_led_state_t state)
{
    if (state > SVC_LED_TRIPLE) return;
    s_state = (led_state_t)state;
}

svc_led_state_t svc_led_get_state(void)
{
    return (svc_led_state_t)s_state;
}

void svc_led_next_state(void)
{
    if (s_state == LED_STATE_TRIPLE) s_state = LED_STATE_SINGLE;
    else s_state = (led_state_t)(s_state + 1);
}

/* External notification entrypoint - called by other services/tasks
 * to inform the LED service about button events. This avoids including
 * the button service header in this module.
 */
void svc_led_handle_button_event(svc_led_button_event_t ev)
{
    /* lazily create the single-item queue if needed */
    if (s_svc_led_ev_q == NULL) {
        s_svc_led_ev_q = xQueueCreate(1, sizeof(svc_led_button_event_t));
    }
    if (s_svc_led_ev_q != NULL) {
        /* overwrite any previously pending value to preserve mailbox semantics */
        xQueueOverwrite(s_svc_led_ev_q, &ev);
    }
}

/* Helper: poll for button events while waiting; returns event or BTN_IDLE */
static svc_led_button_event_t grab_and_clear_pending_event(void)
{
    svc_led_button_event_t ev = SVC_LED_BTN_IDLE;
    if (s_svc_led_ev_q != NULL) {
        if (xQueueReceive(s_svc_led_ev_q, &ev, 0) != pdTRUE) ev = SVC_LED_BTN_IDLE;
    }
    return ev;
}

static svc_led_button_event_t poll_button_event_for(uint32_t wait_ms) {
    svc_led_button_event_t ev = SVC_LED_BTN_IDLE;
    if (s_svc_led_ev_q != NULL) {
        if (xQueueReceive(s_svc_led_ev_q, &ev, pdMS_TO_TICKS(wait_ms)) == pdTRUE) return ev;
    }
    return SVC_LED_BTN_IDLE;
}

static void do_n_blinks(uint8_t n)
{
    for (uint8_t i = 0; i < n; ++i) {
        /* check for button events before each blink */
        svc_led_button_event_t ev = grab_and_clear_pending_event();
        if (ev == SVC_LED_BTN_SHORT_PRESS) { s_state = (s_state==LED_STATE_TRIPLE?LED_STATE_SINGLE:(led_state_t)(s_state+1)); return; }
        if (ev == SVC_LED_BTN_LONG_PRESS) { s_state = LED_STATE_OFF; return; }

        led_on(LED_1);
        ev = poll_button_event_for(BLINK_ON_MS);
        if (ev == SVC_LED_BTN_SHORT_PRESS) { s_state = (s_state==LED_STATE_TRIPLE?LED_STATE_SINGLE:(led_state_t)(s_state+1)); led_off(LED_1); return; }
        if (ev == SVC_LED_BTN_LONG_PRESS) { s_state = LED_STATE_OFF; led_off(LED_1); return; }

        led_off(LED_1);
        ev = poll_button_event_for((i + 1 < n) ? INTER_BLINK_MS : LONG_DELAY_MS);
        if (ev == SVC_LED_BTN_SHORT_PRESS) { s_state = (s_state==LED_STATE_TRIPLE?LED_STATE_SINGLE:(led_state_t)(s_state+1)); return; }
        if (ev == SVC_LED_BTN_LONG_PRESS) { s_state = LED_STATE_OFF; return; }
    }
}

static void svc_led_task(void *arg)
{
    (void)arg;

    /* ensure LED is initialized */
    led_init(LED_1);

    for (;;) {
        switch (s_state) {
            case LED_STATE_OFF:
                led_off(LED_1);
                /* wait for short press to go to SINGLE, long press keeps off */
                for (;;) {
                    svc_led_button_event_t ev = grab_and_clear_pending_event();
                    if (ev == SVC_LED_BTN_SHORT_PRESS) { s_state = LED_STATE_SINGLE; break; }
                    if (ev == SVC_LED_BTN_LONG_PRESS)  { s_state = LED_STATE_OFF; }
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
                break;

            case LED_STATE_SINGLE:
                /* single blink loop */
                do_n_blinks(1);
                break;

            case LED_STATE_DOUBLE:
                do_n_blinks(2);
                break;

            case LED_STATE_TRIPLE:
                do_n_blinks(3);
                break;

            default:
                s_state = LED_STATE_SINGLE;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void svc_led_start(void)
{
    /* create the single-item event queue before any ISR/task may use it */
    if (s_svc_led_ev_q == NULL) {
        s_svc_led_ev_q = xQueueCreate(1, sizeof(svc_led_button_event_t));
    }
    xTaskCreate(svc_led_task, "svc_led", 2048, NULL, tskIDLE_PRIORITY + 1, NULL);
}
