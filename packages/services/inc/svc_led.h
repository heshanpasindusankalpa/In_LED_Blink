#ifndef SVC_LED_H
#define SVC_LED_H

#include "freertos/queue.h"

typedef enum {
	SVC_LED_OFF = 0,
	SVC_LED_SINGLE,
	SVC_LED_DOUBLE,
	SVC_LED_TRIPLE,
} svc_led_state_t;

void svc_led_start(void);


void svc_led_set_state(svc_led_state_t state);


void svc_led_next_state(void);

typedef enum {
	SVC_LED_BTN_IDLE = 0,
	SVC_LED_BTN_SHORT_PRESS,
	SVC_LED_BTN_LONG_PRESS,
} svc_led_button_event_t;


void svc_led_handle_button_event(svc_led_button_event_t ev);

svc_led_state_t svc_led_get_state(void);

/* Process one pending button event from the application button queue.
 * Call from application context (e.g. main loop) to convert ISR-posted
 * button IDs into resolved button events for the LED service.
 */
void led_handler(QueueHandle_t app_queue);

#endif /* SVC_LED_H */