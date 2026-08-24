#ifndef APP_SOC_CMD_UPDATE_FUNC_H__
#define APP_SOC_CMD_UPDATE_FUNC_H__

#include <stdint.h>

typedef enum {
    AP_START_ADDRESS = 0,
    AP_END_ADDRESS,
    PAGE_SIZE,
    MAX_PAGES_PER_PACKET,
    NVRAM_START_ADDRESS,
    NVRAM_END_ADDRESS,
} SOC_CMD_UPDATE_PARAMETER_ID;

typedef enum {
    UPDATE_SUCCESS,
    UPDATE_FAIL
} SOC_CMD_UPDATE_STATUS;

void app_soc_cmd_update_parameter_func(void);
void app_soc_cmd_update_erase_code_func(void);
void app_soc_cmd_update_write_code_func(uint8_t *data, uint16_t len);
void app_soc_cmd_update_start_update_func(void);
void app_soc_cmd_update_end_update_func(void);
void app_soc_cmd_update_operation_mode_func(void);
void app_soc_cmd_update_program_state_func(void);

#endif //APP_SOC_CMD_UPDATE_FUNC_H__