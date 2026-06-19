#ifndef BQ27427_H
#define BQ27427_H

#include "bq27427_config.h"
#include <stdint.h>

/* Re-export public API from existing battery monitor header if present */





/**
 * @file    battery_monitor.h
 * @brief   Battery monitor (BQ27427) driver interface.
 * @version 1.0
 * @date    2026-06-15
 *
 * This file contains definitions and function prototypes for the
 * battery monitor driver used by the platform.
 */

#include "bq27427_config.h"
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * Register addresses (Standard Commands)
 * ------------------------------------------------------------------------- */
#define BQ27427_REG_CONTROL             0x00U   /**< Control() – 2-byte command */
#define BQ27427_REG_TEMPERATURE         0x02U   /**< Temperature() – 0.1 K      */
#define BQ27427_REG_VOLTAGE             0x04U   /**< Voltage() – mV              */
#define BQ27427_REG_FLAGS               0x06U   /**< Flags()                     */
#define BQ27427_REG_NOM_AVAIL_CAP       0x08U   /**< NominalAvailableCapacity()  */
#define BQ27427_REG_FULL_AVAIL_CAP      0x0AU   /**< FullAvailableCapacity()     */
#define BQ27427_REG_REM_CAP             0x0CU   /**< RemainingCapacity()         */
#define BQ27427_REG_FULL_CHG_CAP       0x0EU   /**< FullChargeCapacity()        */
#define BQ27427_REG_AVG_CURRENT         0x10U   /**< AverageCurrent() – mA       */
#define BQ27427_REG_AVG_POWER           0x18U   /**< AveragePower() – mW         */
#define BQ27427_REG_SOC                 0x1CU   /**< StateOfCharge() – %         */
#define BQ27427_REG_INT_TEMP            0x1EU   /**< InternalTemperature()       */
#define BQ27427_REG_SOH                 0x20U   /**< StateOfHealth()             */
#define BQ27427_REG_REM_CAP_UNFILT      0x28U   /**< RemainingCapacityUnfiltered() */
#define BQ27427_REG_REM_CAP_FILT        0x2AU   /**< RemainingCapacityFiltered() */
#define BQ27427_REG_FCC_UNFILT          0x2CU   /**< FullChargeCapacityUnfiltered() */
#define BQ27427_REG_FCC_FILT            0x2EU   /**< FullChargeCapacityFiltered() */
#define BQ27427_REG_SOC_UNFILT          0x30U   /**< StateOfChargeUnfiltered()   */

/* -------------------------------------------------------------------------
 * Extended Data Command addresses
 * ------------------------------------------------------------------------- */
#define BQ27427_REG_DATA_CLASS          0x3EU   /**< DataClass()          */
#define BQ27427_REG_DATA_BLOCK          0x3FU   /**< DataBlock()          */
#define BQ27427_REG_BLOCK_DATA_START    0x40U   /**< BlockData() start    */
#define BQ27427_REG_BLOCK_DATA_CHKSUM   0x60U   /**< BlockDataChecksum()  */
#define BQ27427_REG_BLOCK_DATA_CTRL     0x61U   /**< BlockDataControl()   */

/* -------------------------------------------------------------------------
 * Control() sub-command codes
 * ------------------------------------------------------------------------- */
#define BQ27427_CTRL_STATUS             0x0000U /**< Report device status         */
#define BQ27427_CTRL_DEVICE_TYPE        0x0001U /**< Report device type (0x0427)  */
#define BQ27427_CTRL_FW_VERSION         0x0002U /**< Report firmware version      */
#define BQ27427_CTRL_DM_CODE            0x0004U /**< Report data-memory code      */
#define BQ27427_CTRL_PREV_MACWRITE      0x0007U /**< Previous MAC command         */
#define BQ27427_CTRL_CHEM_ID            0x0008U /**< Chemical identifier          */
#define BQ27427_CTRL_BAT_INSERT         0x000CU /**< Signal battery insertion     */
#define BQ27427_CTRL_BAT_REMOVE         0x000DU /**< Signal battery removal       */
#define BQ27427_CTRL_SET_CFGUPDATE      0x0013U /**< Enter CONFIG UPDATE mode     */
#define BQ27427_CTRL_SMOOTH_SYNC        0x0019U /**< Sync smooth capacity values  */
#define BQ27427_CTRL_SHUTDOWN_ENABLE    0x001BU /**< Enable SHUTDOWN mode         */
#define BQ27427_CTRL_SHUTDOWN           0x001CU /**< Enter SHUTDOWN mode          */
#define BQ27427_CTRL_SEALED             0x0020U /**< Transition to SEALED state   */
#define BQ27427_CTRL_PULSE_SOC_INT      0x0023U /**< Pulse GPOUT pin 1 ms         */
#define BQ27427_CTRL_CHEM_A             0x0030U /**< Switch to Chem ID 3230       */
#define BQ27427_CTRL_CHEM_B             0x0031U /**< Switch to Chem ID 1202       */
#define BQ27427_CTRL_CHEM_C             0x0032U /**< Switch to Chem ID 3142       */
#define BQ27427_CTRL_RESET              0x0041U /**< Full device reset             */
#define BQ27427_CTRL_SOFT_RESET         0x0042U /**< Exit CONFIG UPDATE mode       */

/* -------------------------------------------------------------------------
 * Unseal key (two identical 16-bit words)
 * ------------------------------------------------------------------------- */
#define BQ27427_UNSEAL_KEY              0x8000U

/* -------------------------------------------------------------------------
 * CONTROL_STATUS bit positions (high byte = bits 15-8, low byte = bits 7-0)
 * ------------------------------------------------------------------------- */
#define BQ27427_STATUS_SHUTDOWNEN       (1U << 15)
#define BQ27427_STATUS_WDRESET          (1U << 14)
#define BQ27427_STATUS_SS               (1U << 13) /**< SEALED state active   */
#define BQ27427_STATUS_CALMODE          (1U << 12)
#define BQ27427_STATUS_CCA              (1U << 11)
#define BQ27427_STATUS_BCA              (1U << 10)
#define BQ27427_STATUS_QMAX_UP          (1U << 9)
#define BQ27427_STATUS_RES_UP           (1U << 8)
#define BQ27427_STATUS_INITCOMP         (1U << 7)
#define BQ27427_STATUS_SLEEP            (1U << 4)
#define BQ27427_STATUS_LDMD             (1U << 3)
#define BQ27427_STATUS_RUP_DIS          (1U << 2)
#define BQ27427_STATUS_VOK              (1U << 1)
#define BQ27427_STATUS_CHEMCHANGE       (1U << 0)

/* -------------------------------------------------------------------------
 * FLAGS register bit positions
 * ------------------------------------------------------------------------- */
#define BQ27427_FLAG_OT                 (1U << 15) /**< Over-temperature       */
#define BQ27427_FLAG_UT                 (1U << 14) /**< Under-temperature      */
#define BQ27427_FLAG_FC                 (1U << 9)  /**< Full charge detected   */
#define BQ27427_FLAG_CHG                (1U << 8)  /**< Fast charge allowed    */
#define BQ27427_FLAG_OCVTAKEN           (1U << 7)
#define BQ27427_FLAG_DODCORRECT         (1U << 6)
#define BQ27427_FLAG_ITPOR              (1U << 5)  /**< POR / RESET occurred   */
#define BQ27427_FLAG_CFGUPMODE          (1U << 4)  /**< CONFIG UPDATE active   */
#define BQ27427_FLAG_BAT_DET            (1U << 3)  /**< Battery detected       */
#define BQ27427_FLAG_SOC1               (1U << 2)  /**< SOC threshold 1 hit    */
#define BQ27427_FLAG_SOCF               (1U << 1)  /**< SOC final threshold    */
#define BQ27427_FLAG_DSG                (1U << 0)  /**< Discharging            */

/* -------------------------------------------------------------------------
 * Data-memory subclass IDs referenced by the driver
 * ------------------------------------------------------------------------- */
#define BQ27427_SUBCLASS_SAFETY         2U
#define BQ27427_SUBCLASS_CHG_TERM       36U
#define BQ27427_SUBCLASS_DISCHARGE      49U
#define BQ27427_SUBCLASS_REGISTERS      64U
#define BQ27427_SUBCLASS_IT_CFG         80U
#define BQ27427_SUBCLASS_CURR_THRESH    81U
#define BQ27427_SUBCLASS_STATE          82U
#define BQ27427_SUBCLASS_RA0_RAM        89U
#define BQ27427_SUBCLASS_CHEM_DATA      109U
#define BQ27427_SUBCLASS_CALIB_DATA     104U
#define BQ27427_SUBCLASS_SECURITY       112U

/* -------------------------------------------------------------------------
 * Miscellaneous constants
 * ------------------------------------------------------------------------- */
#define BQ27427_I2C_ADDRESS             0x55U  /**< 7-bit I2C address            */
#define BQ27427_DEVICE_TYPE             0x0427U
#define BQ27427_BLOCK_DATA_SIZE         32U    /**< Extended-command block size   */

/* =========================================================================
 * Initialization / enable / disable
 * ========================================================================= */

/**
 * @brief Initializes the BQ27427 driver instance.
 * @param id The logical sensor identifier (from bq27427_index_t).
 */
void bq27427_init(bq27427_index_t id);

/* =========================================================================
 * Low-level register access
 * ========================================================================= */

/**
 * @brief Writes a 16-bit word to a standard-command register via I2C.
 * @param id    Logical sensor identifier.
 * @param reg   Register address (LSB address of the 2-byte pair).
 * @param data  16-bit value to write (LSB first).
 */
void bq27427_write_word(bq27427_index_t id, uint8_t reg, uint16_t data);

/**
 * @brief Reads a 16-bit word from a standard-command register via I2C.
 * @param id   Logical sensor identifier.
 * @param reg  Register address (LSB address of the 2-byte pair).
 * @return 16-bit value read (LSB first).
 */
uint16_t bq27427_read_word(bq27427_index_t id, uint8_t reg);

/**
 * @brief Writes a single byte to a register via I2C.
 * @param id    Logical sensor identifier.
 * @param reg   Register address.
 * @param data  Byte value to write.
 */
void bq27427_write_byte(bq27427_index_t id, uint8_t reg, uint8_t data);

/**
 * @brief Reads a single byte from a register via I2C.
 * @param id   Logical sensor identifier.
 * @param reg  Register address.
 * @return Byte value read.
 */
uint8_t bq27427_read_byte(bq27427_index_t id, uint8_t reg);

/* =========================================================================
 * Control() sub-command helpers
 * ========================================================================= */

/**
 * @brief Issues a Control() sub-command to the device.
 * @param id          Logical sensor identifier.
 * @param subcommand  One of the BQ27427_CTRL_* constants.
 */
// void bq27427_control(battery_monitor_index_t id, uint16_t subcommand);

// /**
//  * @brief Issues a Control() sub-command and reads back the 16-bit result.
//  * @param id          Logical sensor identifier.
//  * @param subcommand  One of the BQ27427_CTRL_* constants.
//  * @return 16-bit result placed by the gauge at registers 0x00–0x01.
//  */
// uint16_t bq27427_control_read(battery_monitor_index_t id, uint16_t subcommand);

// /* =========================================================================
//  * Access-mode management
//  * ========================================================================= */

// /**
//  * @brief Transitions the gauge from SEALED to UNSEALED mode.
//  * @note  Sends the 0x8000 key word twice consecutively.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_unseal(battery_monitor_index_t id);

// /**
//  * @brief Transitions the gauge from UNSEALED to SEALED mode.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_seal(battery_monitor_index_t id);

// /**
//  * @brief Returns true when the gauge is in SEALED state.
//  * @param id  Logical sensor identifier.
//  * @return true if SEALED, false if UNSEALED.
//  */
// bool bq27427_is_sealed(battery_monitor_index_t id);

// /* =========================================================================
//  * CONFIG UPDATE mode
//  * ========================================================================= */

// /**
//  * @brief Enters CONFIG UPDATE mode (SET_CFGUPDATE sub-command).
//  * @note  Caller must wait ≥ 1100 ms before modifying any parameters.
//  *        The gauge must be UNSEALED before calling this.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_enter_config_update(battery_monitor_index_t id);

// /**
//  * @brief Exits CONFIG UPDATE mode via a SOFT_RESET sub-command.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_exit_config_update(battery_monitor_index_t id);

// /**
//  * @brief Returns true when the CFGUPMODE flag is set.
//  * @param id  Logical sensor identifier.
//  * @return true if CONFIG UPDATE mode is active.
//  */
// bool bq27427_is_config_update(battery_monitor_index_t id);

// /* =========================================================================
//  * Status & flags
//  * ========================================================================= */

// /**
//  * @brief Returns true when the INITCOMP bit is set (initialization complete).
//  * @param id  Logical sensor identifier.
//  * @return true if the gauge has completed initialization.
//  */
// bool bq27427_is_init_complete(battery_monitor_index_t id);

// /**
//  * @brief Reads the raw 16-bit CONTROL_STATUS word.
//  * @param id  Logical sensor identifier.
//  * @return Raw CONTROL_STATUS value.
//  */
// uint16_t bq27427_get_control_status(battery_monitor_index_t id);

// /**
//  * @brief Reads the raw 16-bit Flags() register.
//  * @param id  Logical sensor identifier.
//  * @return Raw flags value; use BQ27427_FLAG_* masks to test individual bits.
//  */
// uint16_t bq27427_get_flags(battery_monitor_index_t id);

// /* =========================================================================
//  * Standard measurement commands
//  * ========================================================================= */

// /**
//  * @brief Reads the cell voltage.
//  * @param id  Logical sensor identifier.
//  * @return Voltage in mV (0–6000 mV).
//  */
// uint16_t bq27427_get_voltage(battery_monitor_index_t id);

// /**
//  * @brief Reads the average current through the sense resistor.
//  * @param id  Logical sensor identifier.
//  * @return Signed current in mA (negative = discharging).
//  */
// int16_t bq27427_get_average_current(battery_monitor_index_t id);

// /**
//  * @brief Reads the average power.
//  * @param id  Logical sensor identifier.
//  * @return Signed power in mW (negative = discharging, positive = charging).
//  */
// int16_t bq27427_get_average_power(battery_monitor_index_t id);

// /**
//  * @brief Reads the temperature reported by the gauge.
//  * @param id  Logical sensor identifier.
//  * @return Temperature in units of 0.1 K.
//  */
// uint16_t bq27427_get_temperature(battery_monitor_index_t id);

// /**
//  * @brief Reads the temperature from the internal sensor specifically.
//  * @param id  Logical sensor identifier.
//  * @return Internal temperature in units of 0.1 K.
//  */
// uint16_t bq27427_get_internal_temperature(battery_monitor_index_t id);

// /**
//  * @brief Reads the state-of-charge.
//  * @param id  Logical sensor identifier.
//  * @return SOC in percent (0–100).
//  */
// uint16_t bq27427_get_soc(battery_monitor_index_t id);

// /**
//  * @brief Reads the unfiltered (true) state-of-charge.
//  * @param id  Logical sensor identifier.
//  * @return SOC in percent, may jump as predictions update.
//  */
// uint16_t bq27427_get_soc_unfiltered(battery_monitor_index_t id);

// /**
//  * @brief Reads the remaining battery capacity (compensated).
//  * @param id  Logical sensor identifier.
//  * @return Remaining capacity in mAh.
//  */
// uint16_t bq27427_get_remaining_capacity(battery_monitor_index_t id);

// /**
//  * @brief Reads the full-charge capacity (compensated).
//  * @param id  Logical sensor identifier.
//  * @return Full-charge capacity in mAh.
//  */
// uint16_t bq27427_get_full_charge_capacity(battery_monitor_index_t id);

// /**
//  * @brief Reads the nominal available capacity (uncompensated).
//  * @param id  Logical sensor identifier.
//  * @return Nominal available capacity in mAh.
//  */
// uint16_t bq27427_get_nominal_available_capacity(battery_monitor_index_t id);

// /**
//  * @brief Reads the full available capacity (uncompensated).
//  * @param id  Logical sensor identifier.
//  * @return Full available capacity in mAh.
//  */
// uint16_t bq27427_get_full_available_capacity(battery_monitor_index_t id);

// /**
//  * @brief Reads the unfiltered remaining capacity.
//  * @param id  Logical sensor identifier.
//  * @return Unfiltered remaining capacity in mAh.
//  */
// uint16_t bq27427_get_remaining_capacity_unfiltered(battery_monitor_index_t id);

// /**
//  * @brief Reads the filtered remaining capacity.
//  * @param id  Logical sensor identifier.
//  * @return Filtered remaining capacity in mAh.
//  */
// uint16_t bq27427_get_remaining_capacity_filtered(battery_monitor_index_t id);

// /**
//  * @brief Reads the unfiltered full-charge capacity.
//  * @param id  Logical sensor identifier.
//  * @return Unfiltered full-charge capacity in mAh.
//  */
// uint16_t bq27427_get_fcc_unfiltered(battery_monitor_index_t id);

// /**
//  * @brief Reads the filtered full-charge capacity.
//  * @param id  Logical sensor identifier.
//  * @return Filtered full-charge capacity in mAh.
//  */
// uint16_t bq27427_get_fcc_filtered(battery_monitor_index_t id);

// /**
//  * @brief Reads the state-of-health percentage.
//  * @param id  Logical sensor identifier.
//  * @return SOH percentage (0–100).
//  */
// uint8_t bq27427_get_soh(battery_monitor_index_t id);

// /**
//  * @brief Reads the state-of-health status byte.
//  * @param id  Logical sensor identifier.
//  * @return SOH status (0x00 = not valid … 0x03 = ready with learned Qmax).
//  */
// uint8_t bq27427_get_soh_status(battery_monitor_index_t id);

// /* =========================================================================
//  * Identification
//  * ========================================================================= */

// /**
//  * @brief Returns the device type (should be 0x0427).
//  * @param id  Logical sensor identifier.
//  * @return 16-bit device type code.
//  */
// uint16_t bq27427_get_device_type(battery_monitor_index_t id);

// /**
//  * @brief Returns the firmware version.
//  * @param id  Logical sensor identifier.
//  * @return 16-bit firmware version code.
//  */
// uint16_t bq27427_get_fw_version(battery_monitor_index_t id);

// /**
//  * @brief Returns the current chemical identifier.
//  * @param id  Logical sensor identifier.
//  * @return Chem ID (3230, 1202, or 3142).
//  */
// uint16_t bq27427_get_chem_id(battery_monitor_index_t id);

// /* =========================================================================
//  * Chemistry profile selection
//  * ========================================================================= */

// /**
//  * @brief Switches to chemistry profile A (Chem ID 3230, 4.35 V default).
//  * @note  Device must be UNSEALED. A SOFT_RESET is issued automatically.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_set_chem_a(battery_monitor_index_t id);

// /**
//  * @brief Switches to chemistry profile B (Chem ID 1202, 4.2 V).
//  * @note  Device must be UNSEALED. A SOFT_RESET is issued automatically.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_set_chem_b(battery_monitor_index_t id);

// /**
//  * @brief Switches to chemistry profile C (Chem ID 3142, 4.4 V).
//  * @note  Device must be UNSEALED. A SOFT_RESET is issued automatically.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_set_chem_c(battery_monitor_index_t id);

// /* =========================================================================
//  * Battery insertion / removal (software signalling, OpConfig[BIE] = 0)
//  * ========================================================================= */

// /**
//  * @brief Signals battery insertion via the BAT_INSERT sub-command.
//  * @note  Only effective when OpConfig[BIE] = 0.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_bat_insert(battery_monitor_index_t id);

// /**
//  * @brief Signals battery removal via the BAT_REMOVE sub-command.
//  * @note  Only effective when OpConfig[BIE] = 0.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_bat_remove(battery_monitor_index_t id);

// /* =========================================================================
//  * Extended data-memory block access
//  * ========================================================================= */

// /**
//  * @brief Writes a full 32-byte block to a data-memory subclass.
//  *
//  * Performs the complete sequence: enable block access → set class/block →
//  * write 32 bytes → compute and write checksum.
//  *
//  * @param id          Logical sensor identifier.
//  * @param subclass_id Data-memory subclass ID (e.g. BQ27427_SUBCLASS_STATE).
//  * @param block       Block offset index (0 = offsets 0–31, 1 = offsets 32–63).
//  * @param data        Pointer to a 32-byte buffer containing the new block data.
//  * @return true on success (checksum verified), false on mismatch.
//  */
// bool bq27427_write_data_block(battery_monitor_index_t id, uint8_t subclass_id,
//                               uint8_t block, const uint8_t *data);

// /**
//  * @brief Reads a full 32-byte block from a data-memory subclass.
//  *
//  * @param id          Logical sensor identifier.
//  * @param subclass_id Data-memory subclass ID.
//  * @param block       Block offset index.
//  * @param data        Pointer to a 32-byte buffer to receive the block data.
//  */
// void bq27427_read_data_block(battery_monitor_index_t id, uint8_t subclass_id,
//                              uint8_t block, uint8_t *data);

// /* =========================================================================
//  * Convenience parameter setters (operate inside CONFIG UPDATE mode)
//  * ========================================================================= */

// /**
//  * @brief Sets the Design Capacity parameter (State subclass, offset 6).
//  * @note  The device must already be in CONFIG UPDATE mode.
//  * @param id       Logical sensor identifier.
//  * @param mah      Design capacity in mAh (0–8000).
//  */
// void bq27427_set_design_capacity(battery_monitor_index_t id, uint16_t mah);

// /**
//  * @brief Sets the Design Energy parameter (State subclass, offset 8).
//  * @note  The device must already be in CONFIG UPDATE mode.
//  * @param id       Logical sensor identifier.
//  * @param mwh      Design energy in mWh (0–32767).
//  */
// void bq27427_set_design_energy(battery_monitor_index_t id, uint16_t mwh);

// /**
//  * @brief Sets the Terminate Voltage parameter (State subclass, offset 10).
//  * @note  The device must already be in CONFIG UPDATE mode.
//  * @param id   Logical sensor identifier.
//  * @param mv   Terminate voltage in mV (2500–3700).
//  */
// void bq27427_set_terminate_voltage(battery_monitor_index_t id, uint16_t mv);

// /* =========================================================================
//  * Power-mode control
//  * ========================================================================= */

// /**
//  * @brief Commands the gauge to enter SHUTDOWN mode.
//  * @note  Device must be UNSEALED. Sends SHUTDOWN_ENABLE then SHUTDOWN.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_shutdown(battery_monitor_index_t id);

// /**
//  * @brief Performs a full device reset (RESET sub-command).
//  * @note  Device must be UNSEALED; best used only in CONFIG UPDATE mode.
//  * @param id  Logical sensor identifier.
//  */
// void bq27427_reset(battery_monitor_index_t id);

/* End of battery_monitor header */

#endif /* BQ27427_H */