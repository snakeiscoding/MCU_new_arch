#include <stddef.h>
#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd_info_func.h"
#include "app_soc_cmd_watchdog_func.h"
#include "app_soc_cmd.h"
#include "hal_soc_cmd_interface.h"
#include "mid_comm_monitor_led.h"
#include "mid_fan.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"
#include "hal_adc.h"


__attribute__((weak)) const char* hal_get_project_name(void) {return 0;}
const char* app_get_project_name(void)
{
    return hal_get_project_name();
}
__attribute__((weak)) uint16_t hal_get_fw_version(void) {return 0;}
uint16_t app_get_fw_version(void)
{
    return hal_get_fw_version();
}
__attribute__((weak)) void mid_enable_soc_lcb_comm_timer(bool enable) {}
void app_soc_cmd_info_version_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t version = app_get_fw_version();
    uint16_t data_len = 2;
    uint8_t data[2];
    data[0] = version & 0x00FF;
    data[1] = (version & 0xFF00) >> 8;
    mid_soc_cmd_send(CMD_INFO_VERSION, CMD_RESULT_CODE_SUCCESS, data, data_len);
    mid_enable_soc_lcb_comm_timer(true);
}

uint16_t app_soc_cmd_info_power_state_current_get(uint8_t target)
{
    uint16_t power_state_current = 0;
    switch((SOC_CMD_CONTROL_POWER_ID)target) {
    case EXT_12V:
        power_state_current = 70;
        break;
    default:
        power_state_current = 0;
        break;
    }
    return power_state_current;
}

void app_soc_cmd_info_power_state_func(uint8_t target, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);
    
    if(len != 1) {
        mid_soc_cmd_send(CMD_INFO_POWER_STATE, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t power_state_current = app_soc_cmd_info_power_state_current_get(target);
    uint16_t data_len = 5;
    uint8_t data[5];
    data[0] = target;
    data[1] = 0x00;
    if(app_soc_cmd_control_power_state_get(target)) {
        data[1] = 0x01;
    }
    data[2] = 0x00;
    data[3] = (power_state_current & 0xFF00) >> 8;
    data[4] = power_state_current & 0x00FF;
    mid_soc_cmd_send(CMD_INFO_POWER_STATE, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

void app_soc_cmd_info_boot_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t data_len = 1;
    uint8_t data[1];
    data[0] = 0x00;
    if(app_soc_cmd_watchdog_get()) {
        data[0] = 0x01;
    }
    mid_soc_cmd_send(CMD_INFO_BOOT, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

__attribute__((weak)) bool hal_get_recovery_state(void) {return 0;}
void app_soc_cmd_info_recovery_state_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t data_len = 1;
    uint8_t data[1];
    data[0] = 0x00;
    if(hal_get_recovery_state()) {
        data[0] = 0x01;
    }
    mid_soc_cmd_send(CMD_INFO_RECOVERY_STATE, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

void app_soc_cmd_info_fan_speed_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);
    
    uint16_t data_len = 3;
    uint8_t data[3];
    uint32_t fan_rpm = mid_get_fan_rpm();
    data[0] = app_get_control_fan_level();
    if(data[0] > 9) {
        data[1] = fan_rpm & 0x00FF;
        data[2] = (fan_rpm & 0xFF00) >> 8;
    }
    else {
        data[1] = 0;
        data[2] = 0;
    }
    mid_soc_cmd_send(CMD_INFO_FAN_SPEED, CMD_RESULT_CODE_SUCCESS, data, data_len);
}

#define VOLTAGE         ((uint8_t)0x00)
#define CURRENT         ((uint8_t)0x01)
#define POWER_STATUS    ((uint8_t)0x02)
__attribute__((weak)) float hal_read_MCB12V_voltage(void) {return 0;}
__attribute__((weak)) float hal_read_EXT12V_voltage(void) {return 0;}
__attribute__((weak)) float hal_read_MCB12V_current(void) {return 0;}
__attribute__((weak)) float hal_read_EXT12V_current(void) {return 0;}
void app_soc_cmd_info_power_monitor_func(uint8_t target, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);
    
    if(len != 1) {
        mid_soc_cmd_send(CMD_INFO_POWER_MONITOR, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }

    switch(target) {
    case VOLTAGE: {
        uint16_t mcb_mV = (uint16_t)(hal_read_MCB12V_voltage() * 1000.0f / 10.0f);
        uint16_t ext_mV = (uint16_t)(hal_read_EXT12V_voltage() * 1000.0f / 10.0f);

        uint16_t data_len = 7;
        uint8_t data[7];        
        data[0] = target;
        data[1] = mcb_mV & 0x00FF;
        data[2] = (mcb_mV >> 8) & 0x00FF;
        data[3] = ext_mV & 0x00FF;
        data[4] = (ext_mV >> 8) & 0x00FF;
        data[5] = 0x00;
        data[6] = 0x00;
        mid_soc_cmd_send(CMD_INFO_POWER_MONITOR, CMD_RESULT_CODE_SUCCESS, data, data_len);
        break;
    }
    case CURRENT: {
        uint16_t mcb_mA = (uint16_t)(hal_read_MCB12V_current() * 1000.0f);
        uint16_t ext_mA = (uint16_t)(hal_read_EXT12V_current() * 1000.0f);

        uint16_t data_len = 5;
        uint8_t data[5];
        data[0] = target;
        data[1] = mcb_mA & 0x00FF;
        data[2] = (mcb_mA >> 8) & 0x00FF;
        data[3] = ext_mA & 0x00FF;
        data[4] = (ext_mA >> 8) & 0x00FF;
        mid_soc_cmd_send(CMD_INFO_POWER_MONITOR, CMD_RESULT_CODE_SUCCESS, data, data_len);
        break;
    }
    case POWER_STATUS: {
        uint16_t data_len = 2;
        uint8_t data[2];
        data[0] = target;
        data[1] = 0x00;
        if(app_soc_cmd_control_power_state_get(target)) {
            data[1] = 0x01;
        }
        mid_soc_cmd_send(CMD_INFO_POWER_MONITOR, CMD_RESULT_CODE_SUCCESS, data, data_len);
        break;
    }
    default:
        mid_soc_cmd_send(CMD_INFO_POWER_MONITOR, CMD_RESULT_CODE_UNKNOWN_COMMAND, NULL, 0);
        break;
    }
}