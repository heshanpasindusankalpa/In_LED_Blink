/**
 * @file    lmp91000.c
 * @brief   LMP91000 device driver implementation.
 * @version 1.0
 * @date    2026-04-27
 */

#include "lmp91000.h"
#include "i2c.h"
#include "gpio.h"

#define bitRead(value, bit) (((value) >> (bit)) & 0x01U)

#define TIA_GAIN_TABLE_SIZE 7U
//#define TIA_BIAS_TABLE_SIZE 14U
#define TIA_ZERO_TABLE_SIZE 3U

static const double TIA_GAIN[TIA_GAIN_TABLE_SIZE] = {
    2750.0, 3500.0, 7000.0, 14000.0, 35000.0, 120000.0, 350000.0,
};

//static const double TIA_BIAS[TIA_BIAS_TABLE_SIZE] = {
//    0.00, 0.01, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.18, 0.20, 0.22, 0.24,
//};

static const double TIA_ZERO[TIA_ZERO_TABLE_SIZE] = {0.20, 0.50, 0.67};

static uint8_t s_gain[LMP91000_COUNT] = {0};
static uint8_t s_zero_index[LMP91000_COUNT] = {1};

/**
 * @brief Initializes the LMP91000 sensor instances.
 */
void lmp91000_init(lmp91000_index_t id) {
    if (id >= LMP91000_COUNT) return;
    
    // Initialize MENB GPIO pin
    gpio_init(g_lmp91000_cfg[id].menb_pin);
    lmp91000_disable(id); // Set high by default
    
    // Initialize I2C Bus
    i2c_init(g_lmp91000_cfg[id].i2c_bus);
}

/**
 * @brief Writes data to a device register via I2C.
 * This common function enables the sensor and writes the specified data to the given register address
 * using the I2C interface. Used to configure or update device registers.
 */
void lmp91000_write_reg(lmp91000_index_t id, uint8_t reg, uint8_t data) {
    if (id >= LMP91000_COUNT) return;
    lmp91000_enable(id);
    i2c_write_register(g_lmp91000_cfg[id].i2c_bus, g_lmp91000_cfg[id].i2c_address, reg, data);
    lmp91000_disable(id);
}

/**
 * @brief Reads a register value from the device via I2C.
 * This function enables the sensor and reads a single byte from the specified register address
 * using the I2C interface. Used to retrieve configuration or status from device registers.
 */
uint8_t lmp91000_read_reg(lmp91000_index_t id, uint8_t reg) {
    if (id >= LMP91000_COUNT) return 0;
    uint8_t data = 0U;
    lmp91000_enable(id);
    i2c_read_register(g_lmp91000_cfg[id].i2c_bus, g_lmp91000_cfg[id].i2c_address, reg, &data, 1U);
    lmp91000_disable(id);
    return data;
}

/**
 * @brief Enables I2C communication by setting MENB low.
 * @note As per the LMP91000 official datasheet: MENB must be set low for the entire duration
 * of any I2C read or write operation. This function pulls the MENB pin low to allow communication.
 */
void lmp91000_enable(lmp91000_index_t id) {
    if (id >= LMP91000_COUNT) return;
    gpio_set_low(g_lmp91000_cfg[id].menb_pin);
}

/**
 * @brief Disables I2C communication by setting MENB high.
 * @note As per the LMP91000 official datasheet: MENB should be set high when I2C communication is not active.
 * This function pulls the MENB pin high to end or block communication with the device.
 */
void lmp91000_disable(lmp91000_index_t id) {
    if (id >= LMP91000_COUNT) return;
    gpio_set_high(g_lmp91000_cfg[id].menb_pin);
}

/**
 * @brief Checks if the LMP91000 device is ready to accept I2C commands.
 * Reads the status register (address 0x00) of the LMP91000 to determine whether the device is ready.
 * @return true if the device is ready, false otherwise.
 * @note Consult page 21, Section 7.6.1 "STATUS -- Status Register (Address 0x00)" of the LMP91000 datasheet for more information.
 * Default state is not ready. The ready bit must be set before issuing further I2C commands.
 * Datasheet extraction: "The STATUS register indicates if the device is ready to accept commands. The READY bit (bit 0) is set to 1 when the device is ready."
 */
bool lmp91000_is_ready(lmp91000_index_t id) {
    return bitRead(lmp91000_read_reg(id, LMP91000_STATUS_REG), 0) == LMP91000_READY;
}

/**
 * @brief Checks if the LMP91000 configuration registers are locked.
 * Reads the LOCK register to determine if the TIACN (0x10) and REFCN (0x11) registers are writable or read-only.
 * - LOCK = 1 (default): TIACN (0x10) and REFCN (0x11) are read-only (locked
 * - LOCK = 0: TIACN and REFCN are writable (unlocked).
 * @return true if the configuration registers are locked, false if they are unlocked.
 * @note See LMP91000 datasheet, page 21-22, Section 7.6.2 "LOCK -- Lock Register (Address 0x12)" for more information.
 * Datasheet extraction: "The LOCK register controls write access to the TIACN and REFCN registers. When LOCK = 1, these registers are read-only. When LOCK = 0, they are writable."
 */
bool lmp91000_is_locked(lmp91000_index_t id) {
    return bitRead(lmp91000_read_reg(id, LMP91000_LOCK_REG), 0) == LMP91000_WRITE_LOCK;
}

/**
 * @brief Locks the configuration registers (TIACN and REFCN) of the LMP91000.
 * Sets the LOCK bit to 1, making the TIACN (0x10) and REFCN (0x11) registers read-only.
 * @note See LMP91000 datasheet, Section 7.6.2 "LOCK -- Lock Register (Address 0x12)".
 * Datasheet extraction: "LOCK = 1 (default): TIACN and REFCN are read-only (locked)."
 */
void lmp91000_lock(lmp91000_index_t id) {
    lmp91000_write_reg(id, LMP91000_LOCK_REG, LMP91000_WRITE_LOCK);
}

/**
 * @brief Unlocks the configuration registers (TIACN and REFCN) of the LMP91000.
 * Sets the LOCK bit to 0, making the TIACN (0x10) and REFCN (0x11) registers writable.
 * @note See LMP91000 datasheet, Section 7.6.2 "LOCK -- Lock Register (Address 0x12)".
 * Datasheet extraction: "LOCK = 0: TIACN and REFCN are writable (unlocked)."
 */
void lmp91000_unlock(lmp91000_index_t id) {
    lmp91000_write_reg(id, LMP91000_LOCK_REG, LMP91000_WRITE_UNLOCK);
}

/**
 * @brief Sets the transimpedance amplifier (TIA) gain of the LMP91000.
 * Configures the TIA gain by updating bits 2-4 of the TIACN register (0x10) according to the provided gain value.
 * Only the lowest 3 bits of gain_value are used. The function first unlocks the register, reads its current value,
 * clears the gain bits, sets the new gain, and writes the updated value back, preserving other register settings.
 * Gain parameter mapping (see datasheet):
 *   0 - 000: External resistor
 *   1 - 001: 2.75 kOhm
 *   2 - 010: 3.5 kOhm
 *   3 - 011: 7 kOhm
 *   4 - 100: 14 kOhm
 *   5 - 101: 35 kOhm
 *   6 - 110: 120 kOhm
 *   7 - 111: 350 kOhm
 * @note See LMP91000 datasheet, page 14 "7.3.1.1 Transimpedance Amplifier" and page 22 "Section 7.6.3 TIACN -- TIA Control Register (Address 0x10)" for more information.
 * Datasheet extraction: "The 3 LSBs of the gain parameter are written to bits 2, 3, and 4 of the TIACN register to set the TIA gain."
 */
void lmp91000_set_gain(lmp91000_index_t id, uint8_t gain_value) {
    if (id >= LMP91000_COUNT) return;
    uint8_t data;
    s_gain[id] = (uint8_t)(gain_value & 0x07U);
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_TIACN_REG);
    data &= (uint8_t)~(0x07U << 2);
    data |= (uint8_t)(s_gain[id] << 2);
    lmp91000_write_reg(id, LMP91000_TIACN_REG, data);
}

/**
 * @brief Returns the currently set TIA gain value in ohms.
 * This function returns the gain value (in ohms) corresponding to the last value set by setGain().
 * - If setGain() has been called, it returns the gain for the current s_gain index.
 * - If setGain() has not been called, s_gain defaults to 0, so it returns the default gain (external resistor).
 * - If s_gain is out of range, returns 0.0 as a safety fallback.
 * @return The TIA gain in ohms, or 0.0 if invalid.
 */
double lmp91000_get_gain(lmp91000_index_t id) {
    if (id >= LMP91000_COUNT || s_gain[id] >= TIA_GAIN_TABLE_SIZE) return 0.0;
    return TIA_GAIN[s_gain[id]];
}

/**
 * @brief Sets the internal load resistor (RLOAD) of the LMP91000.
 * Configures the RLOAD value by updating bits 1:0 of the TIACN register (0x10) according to the provided load value.
 * Only the lowest 2 bits of the load parameter are used. The function first unlocks the register, reads its current value,
 * clears the RLOAD bits, sets the new load value, and writes the updated value back, preserving other register settings.
 * RLOAD parameter mapping (see datasheet):
 *   0 - 00: 10 Ω
 *   1 - 01: 33 Ω
 *   2 - 10: 50 Ω
 *   3 - 11: 100 Ω (default)
 * @note See LMP91000 datasheet, page 14 "7.3.1.1 Transimpedance Amplifier" and page 22 "Section 7.6.3 TIACN -- TIA Control Register (Address 0x10)" for more information.
 * Datasheet extraction: "The 2 LSBs of the load parameter are written to bits 0 and 1 of the TIACN register to set the RLOAD value."
 */
void lmp91000_set_rload(lmp91000_index_t id, uint8_t load) {
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_TIACN_REG);
    data &= (uint8_t)~0x03U;
    data |= (uint8_t)(load & 0x03U);
    lmp91000_write_reg(id, LMP91000_TIACN_REG, data);
}

/**
 * @brief Sets the voltage reference source of the LMP91000.
 * Selects between the internal (Vdd) and external (Vref pin) reference sources by calling
 * setIntRefSource() or setExtRefSource() based on the source parameter.
 * Parameter mapping:
 *   0 - internal reference (Vdd)
 *   1 - external reference (Vref pin)
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)" for more information.
 * Datasheet extraction: "Bit 7 of the REFCN register selects the reference source: 0 = internal (Vdd), 1 = external (Vref pin)."
 */
void lmp91000_set_ref_source(lmp91000_index_t id, uint8_t source) {
    if (source == 0U) lmp91000_set_int_ref_source(id);
    else lmp91000_set_ext_ref_source(id);
}

/**
 * @brief Sets the voltage reference source to internal (Vdd) for the LMP91000.
 * Unlocks the REFCN register for writing, reads its current value, and writes a '0' to bit 7
 * to select the internal reference source (Vdd). All other bits are preserved.
 * - Bit 7 = 0: Internal reference (Vdd)
 * - Bit 7 = 1: External reference (Vref pin)
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)" for more information.
 * Datasheet extraction: "Writing a '0' to bit 7 of the REFCN register selects the internal reference (Vdd)."
 */
void lmp91000_set_int_ref_source(lmp91000_index_t id) {
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data &= (uint8_t)~(1U << 7);
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Sets the voltage reference source to external (Vref pin) for the LMP91000.
 * Unlocks the REFCN register for writing, reads its current value, and writes a '1' to bit 7
 * to select the external reference source (Vref pin). All other bits are preserved.
 * - Bit 7 = 0: Internal reference (Vdd)
 * - Bit 7 = 1: External reference (Vref pin)
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)" for more information.
 * Datasheet extraction: "Writing a '1' to bit 7 of the REFCN register selects the external reference (Vref pin)."
 */
void lmp91000_set_ext_ref_source(lmp91000_index_t id) {
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data |= (uint8_t)(1U << 7);
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Sets the internal zero (IntZ) of the LMP91000 transimpedance amplifier.
 * This function configures the internal zero selection by updating bits 6 and 5 of the REFCN register (0x11).
 * The parameter intZ is a logical value (0-3) corresponding to:
 *   0 - 00 - 20%
 *   1 - 01 - 50%
 *   2 - 10 - 67%
 *   3 - 11 - bypassed
 * The function first unlocks the REFCN register for writing, reads its current value,
 * clears bits 6:5, sets the new IntZ value, and writes the updated value back, preserving other register settings.
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)" for more information.
 * Datasheet extraction: "Bits 6 and 5 of the REFCN register select the internal zero: 00=20%, 01=50%, 10=67%, 11=bypassed."
 */
void lmp91000_set_int_z(lmp91000_index_t id, uint8_t intZ) {
    if (id >= LMP91000_COUNT) return;
    uint8_t data, intz_bits;
    s_zero_index[id] = intZ;
    intz_bits = (uint8_t)((intZ & 0x03U) << 5);
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data &= (uint8_t)~0x60U;
    data |= intz_bits;
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Returns the currently selected internal zero value as a percentage.
 * This function returns the floating-point value (0.20, 0.50, or 0.67) corresponding to the
 * last value set by setIntZ().
 */
double lmp91000_get_int_z(lmp91000_index_t id) {
    if (id >= LMP91000_COUNT || s_zero_index[id] >= TIA_ZERO_TABLE_SIZE) return 0.0;
    return TIA_ZERO[s_zero_index[id]];
}

/**
 * @brief Sets the bias polarity (sign) for the LMP91000.
 * This function selects the bias polarity by setting or clearing bit 4 (BIAS_SIGN) of the REFCN register (0x11):
 *   - 0: Negative bias (V_WE - V_RE < 0V, default)
 *   - 1: Positive bias (V_WE - V_RE > 0V)
 * Calls setNegBias() or setPosBias() to update the register accordingly.
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)", Bit 4 (BIAS_SIGN).
 * Datasheet extraction: "Bit 4: BIAS_SIGN. 0 = Negative (default), 1 = Positive."
 */
void lmp91000_set_bias_sign(lmp91000_index_t id, uint8_t sign) {
    if (sign == 0U) lmp91000_set_neg_bias(id);
    else lmp91000_set_pos_bias(id);
}

/**
 * @brief Sets the bias polarity to negative (V_WE - V_RE < 0V) for the LMP91000.
 * Clears bit 4 (BIAS_SIGN) of the REFCN register (0x11) to select negative bias polarity.
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)", Bit 4 (BIAS_SIGN).
 * Datasheet extraction: "Bit 4: BIAS_SIGN. 0 = Negative (default), 1 = Positive."
 */
void lmp91000_set_neg_bias(lmp91000_index_t id) {
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data &= (uint8_t)~(1U << 4);
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Sets the bias polarity to positive (V_WE - V_RE > 0V) for the LMP91000.
 * Sets bit 4 (BIAS_SIGN) of the REFCN register (0x11) to select positive bias polarity.
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)", Bit 4 (BIAS_SIGN).
 * Datasheet extraction: "Bit 4: BIAS_SIGN. 0 = Negative (default), 1 = Positive."
 */
void lmp91000_set_pos_bias(lmp91000_index_t id) {
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data |= (uint8_t)(1U << 4);
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Sets the bias magnitude for the LMP91000.
 * This function configures the bias magnitude by updating bits 3:0 of the REFCN register (0x11).
 * Only the lowest 4 bits of the bias parameter are used. The function first unlocks the register,
 * reads its current value, clears the bias bits, sets the new bias, and writes the updated value back,
 * preserving other register settings.
 * @note See LMP91000 datasheet, page 22, "Section 7.6.4 REFCN -- Reference Control Register (Address 0x11)", bits 3:0 (BIAS[3:0]).
 * Datasheet extraction: "Bits 3:0: BIAS[3:0]. Sets the bias magnitude as a percentage of Vref."
 */
void lmp91000_set_bias(lmp91000_index_t id, uint8_t bias) {
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data &= (uint8_t)~0x0FU;
    data |= (uint8_t)(bias & 0x0FU);
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Sets the bias magnitude and polarity for the LMP91000.
 * Sets bits 3:0 (BIAS[3:0]) and bit 4 (BIAS_SIGN) of the REFCN register (0x11).
 * - sign: 0 = negative, 1 = positive
 * - bias: allowed values (percentage of reference):
 *   0 = 0% (default), 1 = 1%, ..., 13 = 24%.
 *  Values above 13 are clamped to 0.
 */
void lmp91000_set_bias_with_sign(lmp91000_index_t id, uint8_t bias, int8_t sign) {
    uint8_t sign_bit = (sign > 0) ? 1U : 0U;
    if (bias > 13U) bias = 0U;
    uint8_t data;
    lmp91000_unlock(id);
    data = lmp91000_read_reg(id, LMP91000_REFCN_REG);
    data &= (uint8_t)~0x1FU;
    data |= (uint8_t)((sign_bit << 4) | bias);
    lmp91000_write_reg(id, LMP91000_REFCN_REG, data);
}

/**
 * @brief Sets the FET shorting feature (FET_SHORT) of the LMP91000.
 * Controls bit 7 (FET_SHORT) of the MODECN register (0x12):
 *   - 0: Disabled (default)
 *   - 1: Enabled (shorts the FET)
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bit 7 (FET_SHORT).
 * Datasheet extraction: "Bit 7: FET_SHORT. Shorting FET feature. 0 Disabled (default), 1 Enabled."
 */
void lmp91000_set_fet(lmp91000_index_t id, uint8_t selection) {
    if (selection == 0U) lmp91000_disable_fet(id);
    else lmp91000_enable_fet(id);
}

/**
 * @brief Disables the FET shorting feature (FET_SHORT = 0).
 * Clears bit 7 of the MODECN register (0x12).
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bit 7 (FET_SHORT).
 * Datasheet extraction: "Bit 7: FET_SHORT. 0 Disabled (default)."
 */
void lmp91000_disable_fet(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~(1U << 7);
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Enables the FET shorting feature (FET_SHORT = 1).
 * Sets bit 7 of the MODECN register (0x12).
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bit 7 (FET_SHORT).
 * Datasheet extraction: "Bit 7: FET_SHORT. 1 Enabled."
 */
void lmp91000_enable_fet(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data |= (uint8_t)(1U << 7);
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Dispatches the operating mode setup for the LMP91000.
 */
void lmp91000_set_mode(lmp91000_index_t id, uint8_t mode) {
    switch (mode) {
        case 0U: lmp91000_set_sleep(id); break;
        case 1U: lmp91000_set_two_lead(id); break;
        case 2U: lmp91000_standby(id); break;
        case 3U: lmp91000_set_three_lead(id); break;
        case 4U: lmp91000_measure_cell(id); break;
        case 5U: lmp91000_get_temp(id); break;
        default: break;
    }
}

/**
 * @brief Puts the LMP91000 into deep sleep mode for power conservation.
 * Sets the 3 LSBs of the Mode Control Register (MODECN, 0x12) to 0, placing the device in deep sleep.
 * In this mode, the LMP91000 typically consumes 0.6 μA.
 * @note See LMP91000 datasheet, page 19 Section 7.4 "Device Functional Modes" and page 23 Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)".
 * Datasheet extraction: "Setting the 3 LSBs of the MODECN register to 000 places the device in deep sleep mode (0.6 μA typical)."
 */
void lmp91000_set_sleep(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~0x07U;
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Sets the LMP91000 to 2-lead ground referred galvanic cell mode.
 * Sets bits [2:0] (OP_MODE) of the MODECN register (0x12) to 001.
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bits [2:0] (OP_MODE).
 * Datasheet extraction: "001: 2-lead ground referred galvanic cell."
 */
void lmp91000_set_two_lead(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~0x07U;
    data |= 0x01U;
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Sets the LMP91000 to standby mode.
 * Sets bits [2:0] (OP_MODE) of the MODECN register (0x12) to 010.
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bits [2:0] (OP_MODE).
 * Datasheet extraction: "010: Standby."
 */
void lmp91000_standby(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~0x07U;
    data |= 0x02U;
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Sets the LMP91000 to 3-lead amperometric cell mode.
 * Sets bits [2:0] (OP_MODE) of the MODECN register (0x12) to 011.
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bits [2:0] (OP_MODE).
 * Datasheet extraction: "011: 3-lead amperometric cell."
 */
void lmp91000_set_three_lead(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~0x07U;
    data |= 0x03U;
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Sets the LMP91000 to temperature measurement mode (TIA OFF).
 * Sets bits [2:0] (OP_MODE) of the MODECN register (0x12) to 110.
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bits [2:0] (OP_MODE).
 * Datasheet extraction: "110: Temperature measurement (TIA OFF)."
 */
void lmp91000_measure_cell(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~0x07U;
    data |= 0x06U;
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}

/**
 * @brief Sets the LMP91000 to temperature measurement mode (TIA ON).
 * Sets bits [2:0] (OP_MODE) of the MODECN register (0x12) to 111.
 * @note See LMP91000 datasheet, Section 7.6.5 "MODECN -- Mode Control Register (Address 0x12)", bits [2:0] (OP_MODE).
 * Datasheet extraction: "111: Temperature measurement (TIA ON)."
 */
void lmp91000_get_temp(lmp91000_index_t id) {
    uint8_t data;
    data = lmp91000_read_reg(id, LMP91000_MODECN_REG);
    data &= (uint8_t)~0x07U;
    data |= 0x07U;
    lmp91000_write_reg(id, LMP91000_MODECN_REG, data);
}
