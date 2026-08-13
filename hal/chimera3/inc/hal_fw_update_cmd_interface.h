#ifndef HAL_FW_UPDATE_CMD_INTERFACE_H__
#define HAL_FW_UPDATE_CMD_INTERFACE_H__

#include "app_soc_cmd_update_func.h"
uint32_t hal_get_fw_update_parameter(SOC_CMD_UPDATE_PARAMETER_ID id);
SOC_CMD_UPDATE_STATUS hal_erase_fw_update_partition(void);
SOC_CMD_UPDATE_STATUS hal_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len);
bool hal_verify_fw_update_password(uint8_t *data, uint16_t len);
void hal_start_fw_update(void);
void hal_end_fw_update(void);
SOC_CMD_UPDATE_STATUS hal_get_fw_update_program_state(void);

#endif //HAL_FW_UPDATE_CMD_INTERFACE_H__