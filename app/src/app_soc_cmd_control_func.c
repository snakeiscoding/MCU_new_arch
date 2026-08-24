#include <stddef.h>
#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd.h"
#include "mid_log.h"

extern volatile uint64_t time_1ms;

__attribute__((weak)) uint16_t hal_set_tone_status(uint8_t status) {return 0;}
uint16_t app_control_tone(uint8_t status)
{
    return hal_set_tone_status(status);
}
void app_soc_cmd_control_tone_func(uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 1) {
        mid_soc_cmd_send(CMD_CONTROL_TONE, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t result = app_control_tone(status);
    mid_soc_cmd_send(CMD_CONTROL_TONE, result, NULL, 0);
}

__attribute__((weak)) uint16_t hal_set_fan_status(uint8_t status) {return 0;}
uint8_t fan_level = 0;
uint8_t app_get_control_fan_level(void)
{
    return fan_level;
}
void app_set_control_fan_level(uint8_t level)
{
    if(level < 10) {
        fan_level = 0;
    }
    else {
        fan_level = level;
    }
}
void app_soc_cmd_control_fan_func(uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 1) {
        mid_soc_cmd_send(CMD_CONTROL_FAN, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t result = hal_set_fan_status(status);
    if(result == CMD_RESULT_CODE_SUCCESS) {
        app_set_control_fan_level(status);
    }
    mid_soc_cmd_send(CMD_CONTROL_FAN, result, NULL, 0);
}

__attribute__((weak)) uint16_t hal_set_erp_status(uint8_t status) {return 0;}
void app_soc_cmd_control_erp_func(uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 1) {
        mid_soc_cmd_send(CMD_CONTROL_ERP, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t result = hal_set_erp_status(status);
    mid_soc_cmd_send(CMD_CONTROL_ERP, result, NULL, 0);
}

bool reboot_en = false;
uint64_t reboot_start_time = 0;
uint8_t reboot_timeout = 0;
bool app_soc_cmd_control_reboot_check(void)
{
    if(reboot_en) {
        if(time_1ms - reboot_start_time > (reboot_timeout * 1000)) {
            mid_log_print(LOG_VERBOSE, "SoC and MCU reboot\r\n");
            return true;
        }
    } 
    return false;
}

void app_soc_cmd_control_set_reboot_status(uint8_t status, uint8_t timeout_s)
{
    if(status) {
        reboot_en = true;
        reboot_start_time = time_1ms;
        reboot_timeout = timeout_s;
    }
    else {
        reboot_en = false;
    }
}

void app_soc_cmd_control_reboot_func(uint8_t status, uint8_t time, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 2) {
        mid_soc_cmd_send(CMD_CONTROL_REBOOT, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    app_soc_cmd_control_set_reboot_status(status, time);
    mid_soc_cmd_send(CMD_CONTROL_REBOOT, CMD_RESULT_CODE_SUCCESS, NULL, 0);
}

__attribute__((weak)) uint16_t hal_set_mcu_watchdog(bool status) {return 0;}
bool mcu_watchdog_enable = false;
bool app_soc_cmd_control_get_mcu_watchdog_status(void)
{
    return mcu_watchdog_enable; 
}

void app_soc_cmd_control_mcu_watchdog_func(uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 1) {
        mid_soc_cmd_send(CMD_CONTROL_MCU_WATCHDOG, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    if(status) {
        mcu_watchdog_enable = true;
        hal_set_mcu_watchdog(true);
    }
    else {
        hal_set_mcu_watchdog(false);
    }
    mid_soc_cmd_send(CMD_CONTROL_MCU_WATCHDOG, CMD_RESULT_CODE_SUCCESS, NULL, 0);
}


bool csafe_power_state = false;
bool rfid_power_state = false;
bool tv_tuner_12v_power_state = false;
bool hr_rate_power_state = false;
bool gymkit_power_state = false;
bool usb_5v_power_state = false;
bool ext_12v_state = false;
bool ir_power_state = false;
bool tv_tuner_5v_power_state = false;
bool rs232_5v_power_state = false;
bool app_soc_cmd_control_power_state_get(uint8_t target)
{
    bool power_state = true;
    switch((SOC_CMD_CONTROL_POWER_ID)target) {
    case RS485:
        break;
    case CSAFE:
        power_state = csafe_power_state;
        break;
    case HDMI:
        break;
    case AUDIO_SA:
        break;
    case VOICE:
        break;
    case RFID:
        power_state = rfid_power_state;
        break;
    case QI:
        break;
    case TV_TUNER_12V:
        power_state = tv_tuner_12v_power_state;
        break;
    case CAB:
        break;
    case APPLE_AUTH_IC:
        break;
    case HEART_RATE:
        power_state = hr_rate_power_state;
        break;
    case GYMKIT:
        power_state = gymkit_power_state;
        break;
    case USB_5V:
        power_state = usb_5v_power_state;
        break;
    case EXT_12V:
        power_state = ext_12v_state;
        break;
    case IR_5V:
        power_state = ir_power_state;
        break;
    case TV_TUNER_5V:
        power_state = tv_tuner_5v_power_state;
        break;
    case RS232_5V:
        power_state = rs232_5v_power_state;
        break;
    case UART:
        break;
    default:
        break;
    }

    return power_state;
}

void app_soc_cmd_control_power_state_set(uint8_t target, uint8_t state)
{
    bool power_state = true;
    if(state == 0) {
        power_state = false;
    }
    switch((SOC_CMD_CONTROL_POWER_ID)target) {
    case RS485:
        break;
    case CSAFE:
        csafe_power_state = power_state;
        break;
    case HDMI:
        break;
    case AUDIO_SA:
        break;
    case VOICE:
        break;
    case RFID:
        rfid_power_state = power_state;
        break;
    case QI:
        break;
    case TV_TUNER_12V:
        tv_tuner_12v_power_state = power_state;
        break;
    case CAB:
        break;
    case APPLE_AUTH_IC:
        break;
    case HEART_RATE:
        hr_rate_power_state = power_state;
        break;
    case GYMKIT:
        gymkit_power_state = power_state;
        break;
    case USB_5V:
        usb_5v_power_state = power_state;
        break;
    case EXT_12V:
        ext_12v_state = power_state;
        break;
    case IR_5V:
        ir_power_state = power_state;
        break;
    case TV_TUNER_5V:
        tv_tuner_5v_power_state = power_state;
        break;
    case RS232_5V:
        rs232_5v_power_state = power_state;
        break;
    case UART:
        break;
    default:
        break;
    }
}

__attribute__((weak)) uint16_t hal_set_csafe_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_rfid_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_qi_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_tv_tuner_12v_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_hr_rate_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_gymkit_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_usb_5v_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_ext_12v_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_rs232_5v_power(uint8_t status) {return 0;}
__attribute__((weak)) uint16_t hal_set_tv_tuner_5v_power(uint8_t status) {return 0;}
void app_soc_cmd_control_power_func(uint8_t target, uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t result = CMD_RESULT_CODE_UNKNOWN_COMMAND;
    if(len != 2) {
        mid_soc_cmd_send(CMD_CONTROL_POWER, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    switch((SOC_CMD_CONTROL_POWER_ID)target) {
    case RS485:
        break;
    case CSAFE:
        result = hal_set_csafe_power(status);
        break;
    case HDMI:
        break;
    case AUDIO_SA:
        break;
    case VOICE:
        break;
    case RFID:
        result = hal_set_rfid_power(status);
        break;
    case QI:
        result = hal_set_qi_power(status);
        break;
    case TV_TUNER_12V:
        result = hal_set_tv_tuner_12v_power(status);
        break;
    case CAB:
        break;
    case APPLE_AUTH_IC:
        break;
    case HEART_RATE:
        result = hal_set_hr_rate_power(status);
        break;
    case GYMKIT:
        result = hal_set_gymkit_power(status);
        break;
    case USB_5V:
        result = hal_set_usb_5v_power(status);
        break;
    case EXT_12V:
        result = hal_set_ext_12v_power(status);
        break;
    case IR_5V:
        break;
    case TV_TUNER_5V:
        result = hal_set_tv_tuner_5v_power(status);
        break;
    case RS232_5V:
        result = hal_set_rs232_5v_power(status);
        break;
    case UART:
        break;
    default:
        break;
    }
    app_soc_cmd_control_power_state_set(target, status);
    mid_soc_cmd_send(CMD_CONTROL_POWER, result, NULL, 0);
}

bool soc_debug_mode = false;
uint8_t soc_debug_level = 0;
bool app_soc_cmd_control_debug_mode_get(void)
{
    return soc_debug_mode;
}

uint8_t app_soc_cmd_control_debug_level_get(void)
{
    return soc_debug_level;
}

void app_soc_cmd_control_debug_func(uint8_t enable, uint8_t level, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);
    
    uint16_t result = CMD_RESULT_CODE_SUCCESS;
    if(len != 2) {
        mid_soc_cmd_send(CMD_CONTROL_DEBUG, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    if(enable < 0 || enable > 1 || level < 0 || level > 3) {
        result = CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    else {
        if(enable == 1) {
            soc_debug_mode = true;
        }
        else {
            soc_debug_mode = false;
        }
        soc_debug_level = level;
    }
    mid_soc_cmd_send(CMD_CONTROL_DEBUG, result, NULL, 0);
}

__attribute__((weak)) uint16_t hal_set_qi_status(uint8_t status) {return 0;}
void app_soc_cmd_control_QI_func(uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 1) {
        mid_soc_cmd_send(CMD_CONTROL_QI, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t result = hal_set_qi_status(status);
    mid_soc_cmd_send(CMD_CONTROL_QI, result, NULL, 0);
}

__attribute__((weak)) uint16_t hal_set_led_behavior(uint8_t status) {return 0;}
void app_soc_cmd_control_led_behavior_func(uint8_t status, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 1) {
        mid_soc_cmd_send(CMD_CONTROL_LED_BEHAVIOR, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t result = hal_set_led_behavior(status);
    mid_soc_cmd_send(CMD_CONTROL_LED_BEHAVIOR, result, NULL, 0);
}

__attribute__((weak)) uint16_t hal_set_led_color(uint8_t *data) {return 0;}
void app_soc_cmd_control_led_color_func(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 4) {
        mid_soc_cmd_send(CMD_CONTROL_LED_COLOR, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    uint16_t result = hal_set_led_color(data);
    mid_soc_cmd_send(CMD_CONTROL_LED_COLOR, result, NULL, 0);
}