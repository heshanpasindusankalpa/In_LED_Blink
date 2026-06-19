// #include <stdio.h>
// #include "driver/gpio.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "led.h"
// #include "btn.h"
// #include "platform.h"
// #include "svc_button.h"
// #include "svc_led.h"
// #include "freertos/queue.h"

// /* Application-wide button event queue */
// static QueueHandle_t s_app_btn_queue = NULL;

// /* Button handler logic moved to svc_led service; call via
//  * `led_handler(s_app_btn_queue)` from main loop. */

// void app_main(void)
// {
//     platform_init();

//     /* Initialize button service/context and queue (ISR -> queue) */
//     svc_button_init();

//     s_app_btn_queue = xQueueCreate(10, sizeof(uint32_t));
//     if (s_app_btn_queue == NULL) {
//         printf("Failed to create button queue\n");
//         return;
//     }
//     btn_set_event_queue(s_app_btn_queue);

//     /* Initialize button hardware */
//     btn_init(BTN_1);

//     /* Start LED service */
//     svc_led_start();

//     /* Previously a separate task; now call processing function from main loop */
//     for (;;) {
//         led_handler(s_app_btn_queue);
//         vTaskDelay(pdMS_TO_TICKS(20));
//     }
// }


#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "platform.h"
#include "freertos/queue.h"
#include "bq27427.h"


void app_main(void)
{
	platform_init();

	/* Initialize battery monitor (index 0) */
	bq27427_init(0);

	for (;;) {
		uint16_t voltage = bq27427_read_word(0, BQ27427_REG_VOLTAGE);
		printf("Battery voltage: %u mV\n", (unsigned)voltage);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}