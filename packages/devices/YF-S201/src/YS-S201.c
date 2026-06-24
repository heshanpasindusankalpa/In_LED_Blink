#include "YS-S201.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static volatile uint32_t s_pulse_count = 0;

/* Calibration state and constants */
static volatile uint32_t s_cal_start_count = 0;
/* Default pulses-per-liter derived from datasheet: 7.5 Hz per (L/min) -> 7.5 * 60 = 450 pulses per liter */
static float s_pulses_per_liter = 450.0f;

static void ys_isr(void *arg)
{
    (void)arg;
    /* ISR context: increment pulse counter */
    s_pulse_count++;
}

void ys_init(gpio_index_t pin_id)
{
    gpio_init(pin_id);
    gpio_set_pull(pin_id);
    gpio_config_interrupt(pin_id);
    gpio_register_isr(pin_id, ys_isr, NULL);
}

float ys_measure_lpm(uint32_t sample_ms)
{
    if (sample_ms == 0) {
        return 0.0f;
    }

    uint32_t start = s_pulse_count;
    vTaskDelay(pdMS_TO_TICKS(sample_ms));
    uint32_t end = s_pulse_count;

    uint32_t delta = end - start;
    /* frequency in Hz = pulses / seconds */
    float freq = (float)delta / ((float)sample_ms / 1000.0f);
    /* Convert using calibrated pulses-per-liter: L/min = (freq * 60) / PPL */
    float q_l_min = (freq * 60.0f) / s_pulses_per_liter;
    return q_l_min;
}

uint32_t ys_get_pulse_count(void)
{
    return s_pulse_count;
}

void ys_reset_pulse_count(void)
{
    s_pulse_count = 0;
}

/* ---------------- Calibration helpers ---------------- */
void ys_start_calibration(void)
{
    /* record current counter so we can compute delta later */
    s_cal_start_count = s_pulse_count;
}

uint32_t ys_finish_calibration(void)
{
    uint32_t end = s_pulse_count;
    uint32_t pulses = end - s_cal_start_count;
    return pulses;
}

float ys_compute_ppl_for_one_liter(uint32_t pulses_for_one_liter)
{
    /* For a 1 L pour, the measured pulses equal pulses-per-liter */
    if (pulses_for_one_liter == 0) return 0.0f;
    return (float)pulses_for_one_liter;
}

void ys_set_pulses_per_liter(float ppl)
{
    if (ppl > 0.0f) s_pulses_per_liter = ppl;
}

float ys_get_pulses_per_liter(void)
{
    return s_pulses_per_liter;
}

uint32_t ys_measure_pulses(uint32_t sample_ms)
{
    if (sample_ms == 0) return 0;
    uint32_t start = s_pulse_count;
    vTaskDelay(pdMS_TO_TICKS(sample_ms));
    uint32_t end = s_pulse_count;
    return end - start;
}

uint32_t ys_measure_pulses_for_1l_4s(void)
{
    /* Convenience helper: measure pulses over 4 seconds (4000 ms)
       when pouring ~1 L in ~4s. Use returned value as PPL. */
    return ys_measure_pulses(4000);
}
