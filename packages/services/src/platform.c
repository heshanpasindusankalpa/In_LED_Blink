#include "platform.h"
#include "gpio.h"
#include <stdbool.h>

static bool s_platform_initialized = false;

void platform_init(void)
{
    if (s_platform_initialized) {
        return;
    }
    gpio_isr_service_install(0);
    s_platform_initialized = true;
}
