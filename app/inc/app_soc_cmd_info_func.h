#ifndef APP_SOC_CMD_INFO_FUNC_H__
#define APP_SOC_CMD_INFO_FUNC_H__

#include <stdint.h>

const char* app_get_project_name(void);
uint16_t app_get_fw_version(void);
void app_soc_cmd_info_version_func(void);
void app_soc_cmd_info_power_state_func(uint8_t target, uint16_t len);
void app_soc_cmd_info_boot_func(void);
void app_soc_cmd_info_recovery_state_func(void);
void app_soc_cmd_info_fan_speed_func(void);
void app_soc_cmd_info_power_monitor_func(uint8_t target, uint16_t len);

#endif //APP_SOC_CMD_INFO_FUNC_H__