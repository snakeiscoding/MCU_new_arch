#ifndef HAL_SOC_CMD_INTERFACE_H__
#define HAL_SOC_CMD_INTERFACE_H__

#include <stdbool.h>
#include <stdint.h>

/* hal_control_tone() drives the buzzer pins directly and arms the 20ms
 * release in the timer handler. hal_set_tone_status() is the 0x0001 command
 * entry point; a later keypad path can call hal_control_tone() straight. */
void hal_control_tone(uint8_t status, bool play);
uint16_t hal_set_tone_status(uint8_t status);
/* Replays the latched tone as key press feedback, respecting the mute flag. */
void hal_play_key_tone(void);

uint16_t hal_set_fan_status(uint8_t status);

/* Single-pin power rails, active high. Any non-zero status enables, as in the
 * legacy SetPwrEn(). Ext 12V below is the one rail that needs more than a
 * single pin. */
uint16_t hal_set_csafe_power(uint8_t status);
uint16_t hal_set_rfid_power(uint8_t status);
uint16_t hal_set_tv_tuner_12v_power(uint8_t status);
uint16_t hal_set_hr_rate_power(uint8_t status);

/* Qi. Power target 0x09 switches QI_POWER on/off; command 0x0009 selects the
 * rail - 0x00 = 5V/5W, 0x01 = 12V/15W, per the legacy SetQIPower() encoding.
 * The two rails share one output net, so switching is break-before-make with a
 * 100ms gap. There is no 9V rail. */
uint16_t hal_set_qi_power(uint8_t status);
uint16_t hal_set_qi_status(uint8_t status);

uint16_t hal_set_ext_12v_power(uint8_t status);

/* ERP (0x0004): 0x00 enter via LCB handshake, 0x01 suspend (no-op), 0x02 SoC
 * board off. The remaining four are the mid_erp_flow.c hooks; the STOP mode
 * machinery itself lives in hal_erp_handler.c. */
uint16_t hal_set_erp_status(uint8_t status);
void hal_set_erp_soc_power_off(void);
void hal_set_erp_soc_power_off_success_handler(void);
void hal_lcb_leave_erp_success_handler(void);
void hal_lcb_erp_cmd_fail_handler(void);

const char* hal_get_project_name(void);
uint16_t hal_get_fw_version(void);

void hal_soc_cmd_send(uint8_t *data, uint16_t len);

/* Independent watchdog. Once started the IWDG cannot be stopped, so a disable
 * request is accepted and ignored - the same contract the legacy
 * SetIntoMCUWatchdog() offered. hal_mcu_watchdog_refresh() is a no-op until
 * the watchdog has actually been started. */
uint16_t hal_set_mcu_watchdog(bool status);
void hal_mcu_watchdog_refresh(void);

/* Drops the SoC rail and resets the MCU. Shared by the reboot timer and the
 * SoC watchdog, exactly as the legacy RebootProcess() did. */
void hal_set_soc_mcu_reset(void);

#endif //HAL_SOC_CMD_INTERFACE_H__