#ifndef LMP91000_H
#define LMP91000_H

/**
 * @file    lmp91000.h
 * @brief   LMP91000 device driver interface.
 * @version 1.0
 * @date    2026-04-27
 *
 * This file contains the definitions and function prototypes for
 * configuring and communicating with the LMP91000 analog front end (AFE)
 * designed for micro-power electrochemical sensing applications.
 */

#include "lmp91000_config.h"
#include <stdint.h>
#include <stdbool.h>

#define TEMP_INTERCEPT 1555.0
#define TEMPSLOPE (-8.0)

#define LMP91000_STATUS_REG 0x00U
#define LMP91000_LOCK_REG 0x01U
#define LMP91000_TIACN_REG 0x10U
#define LMP91000_REFCN_REG 0x11U
#define LMP91000_MODECN_REG 0x12U

#define LMP91000_READY 0x01U
#define LMP91000_NOT_READY 0x00U

#define LMP91000_WRITE_LOCK 0x01U
#define LMP91000_WRITE_UNLOCK 0x00U

/**
 * @brief Initializes the LMP91000 sensor instances.
 * @param id The logical sensor identifier (from lmp91000_index_t).
 */
void lmp91000_init(lmp91000_index_t id);

/**
 * @brief Writes data to a device register via I2C.
 * @param id   The logical sensor identifier.
 * @param reg  Register address to write to.
 * @param data Data value to set in the register.
 */
void lmp91000_write_reg(lmp91000_index_t id, uint8_t reg, uint8_t data);

/**
 * @brief Reads a register value from the device via I2C.
 * @param id  The logical sensor identifier.
 * @param reg Register address to read from.
 * @return Value read from the register.
 */
uint8_t lmp91000_read_reg(lmp91000_index_t id, uint8_t reg);

/**
 * @brief Enables I2C communication by setting MENB low.
 * @param id The logical sensor identifier.
 */
void lmp91000_enable(lmp91000_index_t id);

/**
 * @brief Disables I2C communication by setting MENB high.
 * @param id The logical sensor identifier.
 */
void lmp91000_disable(lmp91000_index_t id);

/**
 * @brief Checks if the LMP91000 device is ready to accept I2C commands.
 * @param id The logical sensor identifier.
 * @return true if the device is ready, false otherwise.
 */
bool lmp91000_is_ready(lmp91000_index_t id);

/**
 * @brief Locks the configuration registers (TIACN and REFCN) of the LMP91000.
 * @param id The logical sensor identifier.
 */
void lmp91000_lock(lmp91000_index_t id);

/**
 * @brief Unlocks the configuration registers (TIACN and REFCN) of the LMP91000.
 * @param id The logical sensor identifier.
 */
void lmp91000_unlock(lmp91000_index_t id);

/**
 * @brief Checks if the LMP91000 configuration registers are locked.
 * @param id The logical sensor identifier.
 * @return true if the configuration registers are locked, false if they are unlocked.
 */
bool lmp91000_is_locked(lmp91000_index_t id);

/**
 * @brief Sets the transimpedance amplifier (TIA) gain of the LMP91000.
 * @param id The logical sensor identifier.
 * @param gain_value The desired TIA gain setting (0-7).
 */
void lmp91000_set_gain(lmp91000_index_t id, uint8_t gain_value);

/**
 * @brief Returns the currently set TIA gain value in ohms.
 * @param id The logical sensor identifier.
 * @return The TIA gain in ohms, or 0.0 if invalid.
 */
double lmp91000_get_gain(lmp91000_index_t id);

/**
 * @brief Sets the internal load resistor (RLOAD) of the LMP91000.
 * @param id The logical sensor identifier.
 * @param load The desired RLOAD setting (0-3).
 */
void lmp91000_set_rload(lmp91000_index_t id, uint8_t load);

/**
 * @brief Sets the voltage reference source of the LMP91000.
 * @param id The logical sensor identifier.
 * @param source 0 for internal reference, 1 for external reference.
 */
void lmp91000_set_ref_source(lmp91000_index_t id, uint8_t source);

/**
 * @brief Sets the voltage reference source to internal (Vdd) for the LMP91000.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_int_ref_source(lmp91000_index_t id);

/**
 * @brief Sets the voltage reference source to external (Vref pin) for the LMP91000.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_ext_ref_source(lmp91000_index_t id);

/**
 * @brief Sets the internal zero (IntZ) of the LMP91000 transimpedance amplifier.
 * @param id The logical sensor identifier.
 * @param intZ The internal zero selection (0=20%, 1=50%, 2=67%, 3=bypassed).
 */
void lmp91000_set_int_z(lmp91000_index_t id, uint8_t intZ);

/**
 * @brief Returns the currently selected internal zero value as a percentage.
 * @param id The logical sensor identifier.
 * @return The internal zero value (fractional percentage of Vref).
 */
double lmp91000_get_int_z(lmp91000_index_t id);

/**
 * @brief Sets the bias polarity (sign) for the LMP91000.
 * @param id The logical sensor identifier.
 * @param sign 0 for negative bias, 1 for positive bias.
 */
void lmp91000_set_bias_sign(lmp91000_index_t id, uint8_t sign);

/**
 * @brief Sets the bias polarity to negative (V_WE - V_RE < 0V) for the LMP91000.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_neg_bias(lmp91000_index_t id);

/**
 * @brief Sets the bias polarity to positive (V_WE - V_RE > 0V) for the LMP91000.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_pos_bias(lmp91000_index_t id);

/**
 * @brief Sets the bias magnitude for the LMP91000.
 * @param id The logical sensor identifier.
 * @param bias The desired bias magnitude (0-13 recommended).
 */
void lmp91000_set_bias(lmp91000_index_t id, uint8_t bias);

/**
 * @brief Sets the bias magnitude and polarity for the LMP91000.
 * @param id The logical sensor identifier.
 * @param bias Bias magnitude (0-13).
 * @param sign Bias polarity: 0 = negative, 1 = positive.
 */
void lmp91000_set_bias_with_sign(lmp91000_index_t id, uint8_t bias, int8_t sign);

/**
 * @brief Sets the FET shorting feature (FET_SHORT) of the LMP91000.
 * @param id The logical sensor identifier.
 * @param selection 0 = disable FET short, 1 = enable FET short.
 */
void lmp91000_set_fet(lmp91000_index_t id, uint8_t selection);

/**
 * @brief Disables the FET shorting feature (FET_SHORT = 0).
 * @param id The logical sensor identifier.
 */
void lmp91000_disable_fet(lmp91000_index_t id);

/**
 * @brief Enables the FET shorting feature (FET_SHORT = 1).
 * @param id The logical sensor identifier.
 */
void lmp91000_enable_fet(lmp91000_index_t id);

/**
 * @brief Dispatches the operating mode setup for the LMP91000.
 * @param id The logical sensor identifier.
 * @param mode Mode selector (0-5).
 */
void lmp91000_set_mode(lmp91000_index_t id, uint8_t mode);

/**
 * @brief Puts the LMP91000 into deep sleep mode for power conservation.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_sleep(lmp91000_index_t id);

/**
 * @brief Sets the LMP91000 to 2-lead ground referred galvanic cell mode.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_two_lead(lmp91000_index_t id);

/**
 * @brief Sets the LMP91000 to standby mode.
 * @param id The logical sensor identifier.
 */
void lmp91000_standby(lmp91000_index_t id);

/**
 * @brief Sets the LMP91000 to 3-lead amperometric cell mode.
 * @param id The logical sensor identifier.
 */
void lmp91000_set_three_lead(lmp91000_index_t id);

/**
 * @brief Sets the LMP91000 to temperature measurement mode (TIA OFF).
 * @param id The logical sensor identifier.
 */
void lmp91000_measure_cell(lmp91000_index_t id);

/**
 * @brief Sets the LMP91000 to temperature measurement mode (TIA ON).
 * @param id The logical sensor identifier.
 */
void lmp91000_get_temp(lmp91000_index_t id);

#endif /* LMP91000_H */
