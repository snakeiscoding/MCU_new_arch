#ifndef HAL_SOC_CMD_INTERFACE_H__
#define HAL_SOC_CMD_INTERFACE_H__

#include <stdbool.h>
#include <stdint.h>

uint16_t hal_set_fan_status(uint8_t status);

uint16_t hal_set_ext_12v_power(uint8_t status);

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