#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "btn.h"
#include "platform.h"
#include "svc_button.h"
#include "svc_led.h"
#include "freertos/queue.h"

/* Application-wide button event queue */
static QueueHandle_t s_app_btn_queue = NULL;

/* Button handler: consume ISR-posted button IDs, then use svc_button to
 * determine press type (short/long) and update svc_led state accordingly. */
static void btn_handler_task(void *arg) {
    (void)arg;
    uint32_t btn_id;
    for (;;) {
        if (xQueueReceive(s_app_btn_queue, &btn_id, portMAX_DELAY) == pdPASS) {
            printf("Button event from ISR: %u\n", (unsigned)btn_id);
            if (btn_id >= BTN_COUNT) continue;

            /* poll svc_button until it reports a resolved event for this button */
            for (;;) {
                button_state_t ev = svc_button_event_read((btn_index_t)btn_id);
                if (ev == BTN_SHORT_PRESS) {
                    /* Query current LED state and advance via switch-case */
                    svc_led_state_t st = svc_led_get_state();
                    switch (st) {
                        case SVC_LED_OFF:
                            svc_led_set_state(SVC_LED_SINGLE);
                            break;
                        case SVC_LED_SINGLE:
                            svc_led_set_state(SVC_LED_DOUBLE);
                            break;
                        case SVC_LED_DOUBLE:
                            svc_led_set_state(SVC_LED_TRIPLE);
                            break;
                        case SVC_LED_TRIPLE:
                            svc_led_set_state(SVC_LED_SINGLE);
                            break;
                        default:
                            svc_led_set_state(SVC_LED_SINGLE);
                            break;
                    }
           
                    break;
                } else if (ev == BTN_LONG_PRESS) {
                    svc_led_set_state(SVC_LED_OFF);
                    printf("Long press -> LED off\n");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(20));
            }
        }
    }
}

void app_main(void)
{
    platform_init();

    /* Initialize button service/context and queue (ISR -> queue) */
    svc_button_init();

    s_app_btn_queue = xQueueCreate(10, sizeof(uint32_t));
    if (s_app_btn_queue == NULL) {
        printf("Failed to create button queue\n");
        return;
    }
    btn_set_event_queue(s_app_btn_queue);

    /* Initialize button hardware */
    btn_init(BTN_1);

    /* Start LED service */
    svc_led_start();

    /* Start button handler task to convert ISR events into pattern changes */
    xTaskCreate(btn_handler_task, "btn_handler", 2048, NULL, tskIDLE_PRIORITY + 2, NULL);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
