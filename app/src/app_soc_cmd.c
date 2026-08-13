#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd_event_func.h"
#include "app_soc_cmd_info_func.h"
#include "app_soc_cmd_transceiver_func.h"
#include "app_soc_cmd_update_func.h"
#include "app_soc_cmd_watchdog_func.h"
#include "app_soc_cmd.h"

void app_soc_cmd_handler(const SOC_CMD *p_soc_cmd)
{
    switch((SOC_CMD_ID)p_soc_cmd->cmd) {
    case CMD_CONTROL_TONE:
        app_soc_cmd_control_tone_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_FAN:
        app_soc_cmd_control_fan_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_POWER:
        app_soc_cmd_control_power_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_buffer[1], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_ERP:
        app_soc_cmd_control_erp_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_DEBUG:
        app_soc_cmd_control_debug_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_buffer[1], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_REBOOT:
        app_soc_cmd_control_reboot_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_buffer[1], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_MCU_WATCHDOG:
        app_soc_cmd_control_mcu_watchdog_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_RS485:
        break;
    case CMD_CONTROL_QI:
        app_soc_cmd_control_QI_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_SYSTEM_PARAMETER:
        break;
    case CMD_CONTROL_POWER_MONITOR:
        break;
    case CMD_CONTROL_KEY:
        break;
    case CMD_CONTROL_LED_BEHAVIOR:
        app_soc_cmd_control_led_behavior_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_CONTROL_UART_SWITCH:
        break;
    case CMD_CONTROL_REFRESH_TOUCHPAD_FRAME:
        break;
    case CMD_CONTROL_UPDATE_SPECIFIED_PICTURE:
        break;
    case CMD_CONTROL_LED_COLOR: {
        uint8_t *data_ptr = (uint8_t *)p_soc_cmd->cmd_data_buffer;
        app_soc_cmd_control_led_color_func(data_ptr, p_soc_cmd->cmd_data_len);
        break;
    }
    case CMD_CONTROL_LED_LIGHT_STATUS:
        break;
    case CMD_CONTROL_HAPTIC_MOTOR_CONTROL:
        break;
    case CMD_CONTROL_DATA_DISPLAY:
        break;
    case CMD_CONTROL_SET_RTC:
        break;

    case CMD_WATCHDOG: {
        uint32_t timeout_ms = 0;
        timeout_ms =  p_soc_cmd->cmd_data_buffer[1];
        timeout_ms |= p_soc_cmd->cmd_data_buffer[2] << 8;
        timeout_ms |= p_soc_cmd->cmd_data_buffer[3] << 16;
        timeout_ms |= p_soc_cmd->cmd_data_buffer[4] << 24;
        app_soc_cmd_watchdog_func(p_soc_cmd->cmd_data_buffer[0], timeout_ms, p_soc_cmd->cmd_data_len);
        break;
    }
    case CMD_WATCHDOG_REFRESH:
        app_soc_cmd_watchdog_refresh_func();
        break;

    case CMD_EVENT_KEY:
        break;
    case CMD_EVENT_SAFETY_KEY:
        break;
    case CMD_EVENT_DEBUG_MESSAGE:
        break;
    case CMD_EVENT_LCB_STATUS:
        break;
    case CMD_EVENT_POWER_MONITOR:
        break;
    case CMD_EVENT_QI_STATUS:
        break;
    case CMD_EVENT_HEART_RATE_BPM:
        break;
    case CMD_EVENT_TOUCHPAD_TOUCH:
        break;

    case CMD_INFO_VERSION:
        app_soc_cmd_info_version_func();
        break;
    case CMD_INFO_SAFETY_KEY:
        /*Implemented in app_soc_cmd_event_func.c due to highly similar function logic.*/
        app_soc_cmd_info_safety_key_func();
        break;
    case CMD_INFO_POWER_STATE:
        app_soc_cmd_info_power_state_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_INFO_BOOT:
        app_soc_cmd_info_boot_func();
        break;
    case CMD_INFO_LCB_TYPE:
        break;
    case CMD_INFO_RECOVERY_STATE:
        app_soc_cmd_info_recovery_state_func();
        break;
    case CMD_INFO_FAN_SPEED:
        app_soc_cmd_info_fan_speed_func();
        break;
    case CMD_INFO_SYSTEM_STATE:
        break;
    case CMD_INFO_POWER_MONITOR_STATE:
        break;
    case CMD_INFO_KEY_STATE:
        break;
    case CMD_INFO_POWER_MONITOR:
        app_soc_cmd_info_power_monitor_func(p_soc_cmd->cmd_data_buffer[0], p_soc_cmd->cmd_data_len);
        break;
    case CMD_INFO_RTC_INFORMATION:
        break;

    case CMD_UPDATE_UPDATE_PARAMETER:
        app_soc_cmd_update_parameter_func();
        break;
    case CMD_UPDATE_ERASE_CODE:
        app_soc_cmd_update_erase_code_func();
        break;
    case CMD_UPDATE_WRITE_CODE: {
        uint8_t *data_ptr = (uint8_t *)p_soc_cmd->cmd_data_buffer;
        app_soc_cmd_update_write_code_func(data_ptr, p_soc_cmd->cmd_data_len);
        break;
    }
    case CMD_UPDATE_READ_CODE:
        break;
    case CMD_UPDATE_START_UPDATE: {
        uint8_t *data_ptr = (uint8_t *)p_soc_cmd->cmd_data_buffer;
        app_soc_cmd_update_start_update_func(data_ptr, p_soc_cmd->cmd_data_len);
        break;
    }
    case CMD_UPDATE_END_UPDATE:
        app_soc_cmd_update_end_update_func();
        break;
    case CMD_UPDATE_OPERATION_MODE:
        app_soc_cmd_update_operation_mode_func();
        break;
    case CMD_UPDATE_APPLICATION_NAME:
        break;
    case CMD_UPDATE_PROGRAM_STATE:
        app_soc_cmd_update_program_state_func();
        break;

    case CMD_TRAMSCEIVER_RFID_DATA:
        break;
    case CMD_TRAMSCEIVER_RFID_COMMAND:
        break;
    case CMD_TRAMSCEIVER_SET_CEC_CONFIG:
        break;
    case CMD_TRAMSCEIVER_SET_CEC_COMMAND:
        break;
    case CMD_TRAMSCEIVER_SET_CEC_RESPONSE:
        break;
    case CMD_TRAMSCEIVER_SEND_IR_COMMAND:
        break;
    case CMD_TRAMSCEIVER_SEND_BUMP_SENSOR_COMMAND: {
        uint8_t *data_ptr = (uint8_t *)p_soc_cmd->cmd_data_buffer;
        app_soc_cmd_transceiver_bump_sensor(data_ptr, p_soc_cmd->cmd_data_len);
        break;
    }
    case CMD_TRAMSCEIVER_SEND_CAB_COMMAND: {
        uint8_t *data_ptr = (uint8_t *)p_soc_cmd->cmd_data_buffer;
        app_soc_cmd_transceiver_csafe_cab(data_ptr, p_soc_cmd->cmd_data_len);
        break;
    }
    default:
        break;
    }
}
