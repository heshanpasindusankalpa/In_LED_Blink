#ifndef BQ27427_CONFIG_H
#define BQ27427_CONFIG_H

#include "i2c_config.h"
#include <stdint.h>

typedef uint8_t bq27427_index_t;

#ifndef BQ27427_COUNT
#define BQ27427_COUNT 1
#endif

typedef struct {
    i2c_index_t i2c_bus;
    uint8_t i2c_address;
} bq27427_cfg_t;

extern const bq27427_cfg_t g_bq27427_cfg[BQ27427_COUNT];

#endif /* BQ27427_CONFIG_H */
