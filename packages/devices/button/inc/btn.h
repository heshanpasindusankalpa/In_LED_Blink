#ifndef BTN_H
#define BTN_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "btn_config.h"
#include "gpio.h"


void btn_init(btn_index_t btn_id);
void btn_set_event_queue(QueueHandle_t queue);
bool btn_is_pressed(btn_index_t btn_id);

#endif // BTN_H