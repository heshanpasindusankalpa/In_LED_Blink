#ifndef YS_S201_H
#define YS_S201_H

#include "YS-S201_config.h"
#include "gpio.h"
#include <stdint.h>

/**
 * Initialize the YF-S201 water flow sensor on the specified GPIO pin.
 * The driver registers an ISR to count pulses from the sensor.
 */
void ys_init(gpio_index_t pin_id);

/**
 * Measure the instantaneous flow rate (L/min) by sampling pulses for
 * the given duration in milliseconds. This function blocks for
 * `sample_ms` milliseconds.
 *
 * Returns the measured flow in liters per minute.
 */
float ys_measure_lpm(uint32_t sample_ms);

/**
 * Read raw pulse count since initialization or last reset.
 */
uint32_t ys_get_pulse_count(void);

/**
 * Reset the internal pulse counter to zero.
 */
void ys_reset_pulse_count(void);

/* ---------------- Calibration & helpers ---------------- */
/**
 * Start a calibration period. Records the current pulse counter so that
 * `ys_finish_calibration()` returns the delta pulses observed during the
 * calibration interval.
 */
void ys_start_calibration(void);

/**
 * Finish calibration and return the number of pulses seen since
 * `ys_start_calibration()` was called.
 */
uint32_t ys_finish_calibration(void);

/**
 * Compute pulses-per-liter (PPL) given the number of pulses measured
 * for exactly 1.0 L of water. PPL is used to convert pulses (or frequency)
 * to liters/min. Typical default is 450 PPL (datasheet-derived).
 */
float ys_compute_ppl_for_one_liter(uint32_t pulses_for_one_liter);

/**
 * Set/get the calibration constant: pulses per liter (used to convert
 * pulse frequency to L/min). The driver ships with a reasonable default
 * but calibration is recommended per-sensor.
 */
void ys_set_pulses_per_liter(float ppl);
float ys_get_pulses_per_liter(void);

/**
 * Measure raw pulses for a given interval (ms). Useful for calibration.
 */
uint32_t ys_measure_pulses(uint32_t sample_ms);

/**
 * Convenience: measure pulses for a 1 L pour expected to take ~4 seconds.
 * Returns number of pulses observed during the 4s window — use this value
 * as the PPL (pulses per liter) calibration constant.
 */
uint32_t ys_measure_pulses_for_1l_4s(void);

#endif /* YS_S201_H */
