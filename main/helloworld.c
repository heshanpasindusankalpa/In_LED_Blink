#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "btn.h"
#include "platform.h"
#include "freertos/queue.h"

/* Application-wide button event queue */
static QueueHandle_t s_app_btn_queue = NULL;

static void led1_task(void *arg) {
    (void)arg;
    bool led1_on = false;
    for (;;) {
        if (led1_on) {
            led_off(LED_1);
            led1_on = false;
        } else {
            led_on(LED_1);
            led1_on = true;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
static void btn_handler_task(void *arg) {
    (void)arg;
    uint32_t btn_id;
    bool led2_state = false;
    for (;;) {
        if (xQueueReceive(s_app_btn_queue, &btn_id, portMAX_DELAY) == pdPASS) {
            printf("Button press detected on button %u! Handling...\n", (unsigned)btn_id);
            if (btn_id == BTN_1) {
        
                led2_state = !led2_state;
                if (led2_state) {
                    led_on(LED_2);
                } else {
                    led_off(LED_2);
                }
            }
        }
    }
}

void app_main(void)
{
    platform_init();

    led_init(LED_1);
    led_init(LED_2);

    /* Create the application button event queue (holds up to 10 button IDs) */
    s_app_btn_queue = xQueueCreate(10, sizeof(uint32_t));
    if (s_app_btn_queue == NULL) {
        printf("Failed to create button queue\n");
        return;
    }

    /* Register the queue with the button driver so ISR can post events */
    btn_set_event_queue(s_app_btn_queue);

    btn_init(BTN_1);

    xTaskCreate(led1_task, "led1_task", 2048, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(btn_handler_task, "btn_handler", 2048, NULL, 10, NULL);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

