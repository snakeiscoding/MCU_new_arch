#ifndef HAL_SOC_CMD_INTERFACE_H__
#define HAL_SOC_CMD_INTERFACE_H__

#include <stdbool.h>

uint8_t hal_get_estop_status(void);

void hal_control_tone(uint8_t status, bool power_on);
uint16_t hal_set_tone_status(uint8_t status);

uint16_t hal_set_fan_status(uint8_t status);

void hal_set_erp_soc_power_off_success_handler(void);
void hal_set_erp_soc_power_off(void);
uint16_t hal_set_erp_status(uint8_t status);

/*io power 0x0003*/
uint16_t hal_set_csafe_power(uint8_t status);
uint16_t hal_set_rfid_power(uint8_t status);
uint16_t hal_set_tv_tuner_12v_power(uint8_t status);
uint16_t hal_set_hr_rate_power(uint8_t status);
uint16_t hal_set_gymkit_power(uint8_t status);
uint16_t hal_set_usb_5v_power(uint8_t status);
uint16_t hal_set_ext_12v_power(uint8_t status);
uint16_t hal_set_ir_power(uint8_t status);
uint16_t hal_set_rs232_5v_power(uint8_t status);
uint16_t hal_set_tv_tuner_5v_power(uint8_t status);

uint16_t hal_set_mcu_watchdog(bool enable);

uint16_t hal_set_qi_status(uint8_t status);

uint16_t hal_set_led_behavior(uint8_t status);
void hal_set_erp_led_behavior(bool status);

uint16_t hal_set_led_color(uint8_t *data);

const char* hal_get_project_name(void);
uint16_t hal_get_fw_version(void);
bool hal_get_recovery_state(void);

void hal_soc_cmd_send(uint8_t *data, uint16_t len);
void hal_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len);

#endif //HAL_SOC_CMD_INTERFACE_H__