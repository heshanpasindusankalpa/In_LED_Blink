#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "platform.h"
#include "YS-S201.h"

void app_main(void)
{
	platform_init();

	/* Initialize YS-S201 flow sensor using board mapping */
	ys_init(g_ys_cfg[0].pin_id);

	/* === Calibration: set your measured pulses-per-liter here ===
	 * If you already know your sensor's PPL (pulses per liter), set
	 * USER_PPL_VALUE to that value (e.g. 470.0f). If left 0.0f, the
	 * program will perform a one-time 4-second measurement that you
	 * should use while pouring exactly 1.0 L (aim for ~4 seconds).
	 * The measured pulses will be applied as the calibration constant.
	 */
#define USER_PPL_VALUE 260.0f

	if (USER_PPL_VALUE > 0.0f) {
		ys_set_pulses_per_liter(USER_PPL_VALUE);
		printf("YS-S201: using hardcoded calibration PPL=%.2f pulses/L\n", ys_get_pulses_per_liter());
	} else {
		printf("YS-S201: no hardcoded PPL set. Performing one-time 4s measure for 1L calibration\n");
		printf("Prepare to pour 1.0 L through the sensor in ~4 seconds. Starting soon...\n");
		vTaskDelay(pdMS_TO_TICKS(2000)); /* small delay to let user prepare */
		uint32_t pulses = ys_measure_pulses_for_1l_4s();
		float ppl = ys_compute_ppl_for_one_liter(pulses);
		printf("Calibration: measured %" PRIu32 " pulses over 4s -> PPL=%.2f pulses/L\n", pulses, ppl);
		ys_set_pulses_per_liter(ppl);
		printf("Applied calibration PPL=%.2f\n", ys_get_pulses_per_liter());
	}

	for (;;) {
		/* Sample for 1 second and report L/min */
		float lpm = ys_measure_lpm(1000);
		printf("YS-S201 flow: %.2f L/min\n", lpm);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

