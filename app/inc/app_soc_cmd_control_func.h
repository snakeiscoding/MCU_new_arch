#ifndef APP_SOC_CMD_CONTROL_FUNC_H__
#define APP_SOC_CMD_CONTROL_FUNC_H__

#include <stdbool.h>
#include <stdint.h>

uint16_t app_control_tone(uint8_t status);
void app_soc_cmd_control_tone_func(uint8_t status, uint16_t len);
uint8_t app_get_control_fan_level(void);
void app_soc_cmd_control_fan_func(uint8_t status, uint16_t len);
bool app_soc_cmd_control_power_state_get(uint8_t target);
void app_soc_cmd_control_power_state_set(uint8_t target, uint8_t status);
void app_soc_cmd_control_power_func(uint8_t target, uint8_t status, uint16_t len);
void app_soc_cmd_control_erp_func(uint8_t status, uint16_t len);
void app_soc_cmd_control_debug_func(uint8_t enable, uint8_t level, uint16_t len);

bool app_soc_cmd_control_reboot_check(void);
void app_soc_cmd_control_reboot_func(uint8_t status, uint8_t timeout_s, uint16_t len);

bool app_soc_cmd_control_get_mcu_watchdog_status(void);
void app_soc_cmd_control_mcu_watchdog_func(uint8_t status, uint16_t len);

void app_soc_cmd_control_QI_func(uint8_t status, uint16_t len);
void app_soc_cmd_control_led_behavior_func(uint8_t status, uint16_t len);

void app_soc_cmd_control_led_color_func(uint8_t *data, uint16_t len);

#endif //APP_SOC_CMD_CONTROL_FUNC_H__