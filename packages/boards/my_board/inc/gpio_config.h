#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#include "gpio_types.h"
#include "driver/gpio.h"
#include "pin_config.h"

typedef void (*app_gpio_isr_t)(void *arg);

typedef struct {
  uint32_t pin_num;      
  gpio_mode_t direction; 
  gpio_int_type_t intr_type; 
  gpio_pull_mode_t pull;     
} gpio_cfg_t;

extern const gpio_cfg_t g_gpio_cfg[GPIO_CFG_COUNT];

typedef struct {
  app_gpio_isr_t handler;
  void *args;
} app_gpio_isr_state_t;

extern app_gpio_isr_state_t g_gpio_isr_table[GPIO_CFG_COUNT];

#endif /* GPIO_CONFIG_H */



