#ifndef HAL_SOC_CMD_INTERFACE_H__
#define HAL_SOC_CMD_INTERFACE_H__

uint16_t hal_set_fan_status(uint8_t status);

uint16_t hal_set_ext_12v_power(uint8_t status);

const char* hal_get_project_name(void);
uint16_t hal_get_fw_version(void);

void hal_soc_cmd_send(uint8_t *data, uint16_t len);

#endif //HAL_SOC_CMD_INTERFACE_H__