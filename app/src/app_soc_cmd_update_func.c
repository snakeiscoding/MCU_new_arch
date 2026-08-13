#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "app_soc_cmd_update_func.h"
#include "app_soc_cmd.h"
#include "hal_fw_update_cmd_interface.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

#define NORMAL_MODE ((uint8_t)0x00)
#define UPDATE_MODE ((uint8_t)0x01)

uint8_t operation_mode = NORMAL_MODE;
void app_soc_cmd_update_set_operation_mode(int mode)
{
    operation_mode = mode;
}

uint8_t app_soc_cmd_update_get_operation_mode(void)
{
    return operation_mode;
}

__attribute__((weak)) uint32_t hal_get_fw_update_parameter(SOC_CMD_UPDATE_PARAMETER_ID id) {return 0;}
void app_soc_cmd_update_parameter_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t data_len = 21;
    uint8_t data[21];
    uint32_t *p32 = (uint32_t *)data;
    *(p32++) = hal_get_fw_update_parameter(AP_START_ADDRESS);
    *(p32++) = hal_get_fw_update_parameter(AP_END_ADDRESS);
    *(p32++) = hal_get_fw_update_parameter(PAGE_SIZE);
    data[12] = (uint8_t)hal_get_fw_update_parameter(MAX_PAGES_PER_PACKET);
    uint32_t *p32_next = (uint32_t *)&data[13];
    *(p32_next++) = hal_get_fw_update_parameter(NVRAM_START_ADDRESS);
    *(p32_next)   = hal_get_fw_update_parameter(NVRAM_END_ADDRESS);
    mid_soc_cmd_send(CMD_UPDATE_UPDATE_PARAMETER, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

static bool erase_code_ready = false;
__attribute__((weak)) SOC_CMD_UPDATE_STATUS hal_erase_fw_update_partition(void) {return UPDATE_SUCCESS;}
void app_soc_cmd_update_erase_code_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    SOC_CMD_UPDATE_STATUS status = UPDATE_SUCCESS;
    if(!erase_code_ready) {
        status = hal_erase_fw_update_partition();
    }
    if(status == UPDATE_FAIL) {
        mid_soc_cmd_send(CMD_UPDATE_ERASE_CODE, CMD_RESULT_CODE_FAILURE, NULL, 0);
    }
    else {
        erase_code_ready = true;
        mid_soc_cmd_send(CMD_UPDATE_ERASE_CODE, CMD_RESULT_CODE_SUCCESS, NULL, 0);
    }
}

__attribute__((weak)) SOC_CMD_UPDATE_STATUS hal_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len) {return UPDATE_SUCCESS;}
void app_soc_cmd_update_write_code_func(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len < 8) {
        mid_soc_cmd_send(CMD_UPDATE_WRITE_CODE, CMD_RESULT_CODE_FAILURE, NULL, 0);
        return;
    }

    SOC_CMD_UPDATE_STATUS status;
    uint32_t start_addr = 0;
    uint32_t write_len = 0;
    memcpy(&start_addr, &data[0], sizeof(start_addr));
    memcpy(&write_len, &data[4], sizeof(write_len));
    status = hal_write_fw_update_data(start_addr, &data[8], write_len);

    if(status == UPDATE_FAIL) {
        mid_soc_cmd_send(CMD_UPDATE_WRITE_CODE, CMD_RESULT_CODE_FAILURE, NULL, 0);
    }
    else {
        mid_soc_cmd_send(CMD_UPDATE_WRITE_CODE, CMD_RESULT_CODE_SUCCESS, NULL, 0);
    }
}
/* Boards without a password keep the current behaviour: accept everything. */
__attribute__((weak)) bool hal_verify_fw_update_password(uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;
    return true;
}

__attribute__((weak)) void hal_start_fw_update(void) {}
void app_soc_cmd_update_start_update_func(uint8_t *data, uint16_t len)
 {
     mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);
 
    if(!hal_verify_fw_update_password(data, len)) {
        mid_log_print(LOG_ERROR, "fw update start rejected: bad password\r\n");
        mid_soc_cmd_send(CMD_UPDATE_START_UPDATE, CMD_RESULT_CODE_INVALID_PASSWORD, NULL, 0);
        return;
    }

    if(app_soc_cmd_update_get_operation_mode() != UPDATE_MODE)
    {
        app_soc_cmd_update_set_operation_mode(UPDATE_MODE);
    }
    mid_soc_cmd_send(CMD_UPDATE_START_UPDATE, CMD_RESULT_CODE_SUCCESS, NULL, 0);
    hal_start_fw_update();
 }

__attribute__((weak)) void hal_end_fw_update(void) {}
void app_soc_cmd_update_end_update_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    mid_soc_cmd_send(CMD_UPDATE_END_UPDATE, CMD_RESULT_CODE_SUCCESS, NULL, 0);
    hal_end_fw_update();
}

void app_soc_cmd_update_operation_mode_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);
    uint16_t data_len = 1;
    uint8_t data[1];
    data[0] = app_soc_cmd_update_get_operation_mode();
    mid_soc_cmd_send(CMD_UPDATE_OPERATION_MODE, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

__attribute__((weak)) SOC_CMD_UPDATE_STATUS hal_get_fw_update_program_state(void) {return UPDATE_SUCCESS;}
#define AP_NO_UPDATED   ((uint8_t)0x00)
#define AP_UPDATED      ((uint8_t)0x01)
void app_soc_cmd_update_program_state_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    SOC_CMD_UPDATE_STATUS status;
    uint16_t data_len = 1;
    uint8_t data[1];
    status = hal_get_fw_update_program_state();

    if(status == UPDATE_FAIL) {
        data[0] = AP_NO_UPDATED;
    }
    else {
        data[0] = AP_UPDATED;
    }
    mid_soc_cmd_send(CMD_UPDATE_PROGRAM_STATE, CMD_RESULT_CODE_SUCCESS, data, data_len);
}
