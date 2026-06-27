#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_proto.h"
#include "uart.h"
#include "uart_config.h"



void app_main(void)
{
    /* Open the PC UART channel so PuTTY can receive readable hex output. */
    uart_init(UART_CH_PC);

    for (;;) {
        uart_proto_demo_send();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

