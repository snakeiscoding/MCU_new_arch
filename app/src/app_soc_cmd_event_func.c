#include "app_soc_cmd_event_func.h"
#include "app_soc_cmd.h"
#include "hal_soc_cmd_interface.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

#define ESTOP_CLEAR     ((uint8_t)0x00)
#define ESTOP_TRIGGER   ((uint8_t)0x01)
__attribute__((weak)) uint8_t hal_get_estop_status(void) {return 0;}
void app_soc_cmd_event_estop_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t data_len = 1;
    uint8_t data[1];
    if(hal_get_estop_status() == 1) {
        data[0] = ESTOP_CLEAR;
    }
    else if(hal_get_estop_status() == 0) {
        data[0] = ESTOP_TRIGGER;
    }
    mid_soc_cmd_send(CMD_EVENT_SAFETY_KEY, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

void app_soc_cmd_info_safety_key_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t data_len = 1;
    uint8_t data[1];
    if(hal_get_estop_status() == 1) {
        data[0] = ESTOP_CLEAR;
    }
    else if(hal_get_estop_status() == 0) {
        data[0] = ESTOP_TRIGGER;
    }
    mid_soc_cmd_send(CMD_INFO_SAFETY_KEY, CMD_RESULT_CODE_SUCCESS, data, data_len);
}