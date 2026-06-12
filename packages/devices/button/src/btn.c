
#include "btn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static QueueHandle_t s_btn_queue = NULL;

static TickType_t s_last_isr_time[BTN_COUNT] = { 0 };


static void btn_callback(void *arg) {
  uint32_t btn_id = (uint32_t)(uintptr_t)arg;
  TickType_t now = xTaskGetTickCountFromISR();

  if (btn_id >= BTN_COUNT) {
    return;
  }


  if ((now - s_last_isr_time[btn_id]) < pdMS_TO_TICKS(200)) {
    return;
  }
  s_last_isr_time[btn_id] = now;

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (s_btn_queue != NULL) {
    /* Send the button index to the application task for processing */
    xQueueSendFromISR(s_btn_queue, &btn_id, &xHigherPriorityTaskWoken);
    
    if (xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
}

void btn_set_event_queue(QueueHandle_t queue)
{
  s_btn_queue = queue;
}


void btn_init(btn_index_t btn_id) {
  if (btn_id >= BTN_COUNT) {
    return;
  }
  /* Setup physical GPIO properties */
  gpio_init(g_btn_gpio_cfg[btn_id].pin_id);
  gpio_set_pull(g_btn_gpio_cfg[btn_id].pin_id);

  gpio_config_interrupt(g_btn_gpio_cfg[btn_id].pin_id);
  gpio_register_isr(g_btn_gpio_cfg[btn_id].pin_id, btn_callback, (void *)(uintptr_t)btn_id);
  
}

bool btn_is_pressed(btn_index_t btn_id) {
  if (btn_id >= BTN_COUNT) {
    return false;
  }
  uint8_t level;
  gpio_read(g_btn_gpio_cfg[btn_id].pin_id, &level);
  
  /* Returns true if level is LOW (assuming Active-Low button configuration) */
  return (level == 0);
}

