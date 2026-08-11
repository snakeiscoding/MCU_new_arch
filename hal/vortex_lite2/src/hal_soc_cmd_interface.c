#include <stdio.h>
#include "hal_config.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_power.h"
#include "hal_pwm.h"
#include "hal_soc_cmd_interface.h"
#include "hal_systick.h"
#include "hal_timer_handler.h"
#include "hal_uart.h"
#include "mid_comm_monitor_led.h"
#include "mid_erp_flow.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

uint8_t hal_get_estop_status(void)
{
    return hal_gpio_get(MCU_SW_POSITION_GPIO);
}

#define TOGGLE_TONE ((uint8_t)0x00)
#define LOW_TONE    ((uint8_t)0x01)
#define MID_TONE    ((uint8_t)0x02)
#define HIGH_TONE   ((uint8_t)0x03)

bool tone_status_on = true;
void hal_control_tone(uint8_t status, bool power_on)
{
    if(!power_on) {
        hal_gpio_set(MCU_TR1_GPIO, GPIO_LOW);
        hal_gpio_set(MCU_TR2_GPIO, GPIO_LOW);
        hal_gpio_set(MCU_TR3_GPIO, GPIO_LOW);
    } 
    else {
        if(status == LOW_TONE) {
            hal_gpio_set(MCU_TR1_GPIO, GPIO_HIGH);
            hal_gpio_set(MCU_TR2_GPIO, GPIO_LOW);
            hal_gpio_set(MCU_TR3_GPIO, GPIO_LOW);
        }
        else if(status == MID_TONE) {
            hal_gpio_set(MCU_TR1_GPIO, GPIO_LOW);
            hal_gpio_set(MCU_TR2_GPIO, GPIO_HIGH);
            hal_gpio_set(MCU_TR3_GPIO, GPIO_LOW);
        }
        else if(status == HIGH_TONE) {
            hal_gpio_set(MCU_TR1_GPIO, GPIO_LOW);
            hal_gpio_set(MCU_TR2_GPIO, GPIO_LOW);
            hal_gpio_set(MCU_TR3_GPIO, GPIO_HIGH);
        }
        hal_set_tone_start_timer();
    }
}
uint16_t hal_set_tone_status(uint8_t status)
{
    if(status == TOGGLE_TONE) {
        tone_status_on = !tone_status_on;
    }
    else if(status == LOW_TONE && tone_status_on) {
        hal_control_tone(LOW_TONE, true);
    }
    else if(status == MID_TONE && tone_status_on) {
        hal_control_tone(MID_TONE, true);
    }
    else if(status == HIGH_TONE && tone_status_on) {
        hal_control_tone(HIGH_TONE, true);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_fan_status(uint8_t status)
{
    if(status > 100) {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    else if(status > 9) {
        hal_gpio_set(FAN_PWR_EN_GPIO, GPIO_HIGH);
        hal_pwm_set_pulse(PWM_PA6, 100 - status);
        hal_pwm_set_enable(PWM_PA6, true);
    }
    else {
        hal_gpio_set(FAN_PWR_EN_GPIO, GPIO_LOW);
        hal_pwm_set_pulse(PWM_PA6, 100);
        hal_pwm_set_enable(PWM_PA6, false);
    }
    return CMD_RESULT_CODE_SUCCESS;
}

#define ERP_ON              ((uint8_t)0x00)
#define ERP_SUSPEND         ((uint8_t)0x01)
#define ERP_POWER_OFF_UCB   ((uint8_t)0x02)
void hal_set_erp_soc_power_off_success_handler(void)
{
    hal_set_erp_power_off();
}

void hal_set_erp_soc_power_off(void)
{
    hal_set_soc_power_off();
}

uint16_t hal_set_erp_status(uint8_t status)
{
    if(status == ERP_ON) {
        //mid_erp_status_set(LCB_ENTER_ERP_STATUS);
        /* Note: Run 'lcb init' for ERP access in non-GUI mode */
        mid_erp_status_set(LCB_INIT_STATUS);
    }
    else if(status == ERP_SUSPEND) {
    
    }
    else if(status == ERP_POWER_OFF_UCB) {
        hal_set_soc_power_off();
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

#define POWER_DISABLE       ((uint8_t)0x00)
#define POWER_ENABLE        ((uint8_t)0x01)
#define POWER_SWITCH        ((uint8_t)0x02)

uint16_t hal_set_csafe_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(MCU_CSAFE_PEN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(MCU_CSAFE_PEN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_rfid_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(MCU_RFID_PEN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(MCU_RFID_PEN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_tv_tuner_12v_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(TV_12V_EN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(TV_12V_EN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_hr_rate_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(MCU_HR_PEN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(MCU_HR_PEN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_gymkit_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(MCU_USB30_PWREN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(MCU_USB30_PWREN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_usb_5v_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(MCU_USB30_PWREN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(MCU_USB30_PWREN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_ext_12v_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(EXIT_PW_EN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(EXIT_PW_EN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_ir_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(MCU_ERP_PEN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(MCU_ERP_PEN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_rs232_5v_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(RS232_PEN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(RS232_PEN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_tv_tuner_5v_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(TV_5V_EN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(TV_5V_EN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_mcu_watchdog(bool enable)
{
    if(enable) {
        fwdgt_write_enable();
        fwdgt_config(1250, FWDGT_PSC_DIV256);
        fwdgt_counter_reload();
        fwdgt_enable();
    }
    return CMD_RESULT_CODE_SUCCESS;
}

#define QI_5V   ((uint8_t)0x00)
#define QI_9V   ((uint8_t)0x01)
#define QI_12V  ((uint8_t)0x02)
// #define QI_TXRX_LOOPBACK_DEBUG
uint16_t hal_set_qi_status(uint8_t status)
{
    if(status == QI_9V) {
        hal_gpio_set(QI_12VPEN_GPIO, GPIO_LOW);
    }
    else if(status == QI_12V) {
        hal_gpio_set(QI_12VPEN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
#ifdef QI_TXRX_LOOPBACK_DEBUG
    uint8_t data[32];
    int len = snprintf((char *)data, sizeof(data), "set QI to %d\r\n", status);
    hal_uart_send(UART_ID_6, data, len);
#endif
    return CMD_RESULT_CODE_SUCCESS;
}

#define LED_BEHAVIOR_1  ((uint8_t)0x00)
#define LED_BEHAVIOR_2  ((uint8_t)0x01)
#define LED_BEHAVIOR_3  ((uint8_t)0x02)
#define LED_BEHAVIOR_4  ((uint8_t)0x03)
uint8_t led_behavior_status = LED_BEHAVIOR_1;
uint8_t hal_get_led_behavior()
{
    return led_behavior_status;
}
uint16_t hal_set_led_behavior(uint8_t status)
{
    if(status == LED_BEHAVIOR_1) {
        led_behavior_status = status;
        hal_gpio_set(MCU_ERP_LED_GPIO, GPIO_LOW);
        hal_gpio_set(ERP_GREEN_GPIO, GPIO_LOW);
    }
    else if(status == LED_BEHAVIOR_2) {
        led_behavior_status = status;
        hal_gpio_set(MCU_ERP_LED_GPIO, GPIO_HIGH);
        hal_gpio_set(ERP_GREEN_GPIO, GPIO_HIGH);
    }
    else if(status == LED_BEHAVIOR_3) {
        hal_gpio_set(ERP_WHITE_GPIO, GPIO_LOW);
    }
    else if(status == LED_BEHAVIOR_4) {
        hal_gpio_set(ERP_WHITE_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

void hal_set_erp_led_behavior(bool status)
{
    if(status) {
        hal_gpio_set(MCU_ERP_LED_GPIO, GPIO_HIGH);
        hal_gpio_set(ERP_GREEN_GPIO, GPIO_HIGH);
    }
    else {
        hal_gpio_set(MCU_ERP_LED_GPIO, GPIO_LOW);
        hal_gpio_set(ERP_GREEN_GPIO, GPIO_LOW);
    }
}

#define LED_CONSOLE     ((uint8_t)0x00)
#define LED_SIDE        ((uint8_t)0x01)
#define LED_BUTTON1     ((uint8_t)0x02)
#define LED_BUTTON2     ((uint8_t)0x03)
#define LED_All         ((uint8_t)0x04)

#define RED_CON_LED     ((uint16_t)(1U << 11))
#define BLUE_CON_LED    ((uint16_t)(1U << 12))
#define GREEN_CON_LED   ((uint16_t)(1U << 15))
extern uint32_t rgb_led_pwm_buf[256];
void hal_update_rgb_led_pwm_buf(uint8_t red_duty, uint8_t green_duty, uint8_t blue_duty)
{
    for(uint32_t i = 0; i < 256; i++) {
        uint32_t set_bits = 0;
        uint32_t clr_bits = 0;

        if(i < red_duty) {
            set_bits |= RED_CON_LED;
        }
        else {
            clr_bits |= RED_CON_LED;
        }
        if(i < green_duty) {
            set_bits |= GREEN_CON_LED;
        }
        else {
            clr_bits |= GREEN_CON_LED;
        }
        if(i < blue_duty) {
            set_bits |= BLUE_CON_LED;
        }
        else {
            clr_bits |= BLUE_CON_LED;
        }
        rgb_led_pwm_buf[i] = (clr_bits << 16) | set_bits;
    }
}

uint16_t hal_set_led_color(uint8_t *data)
{
    if(data[0] > LED_All) {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }

    if(data[0] == LED_CONSOLE || data[0] == LED_All) {
        hal_update_rgb_led_pwm_buf(data[1], data[2], data[3]);
    }

    return CMD_RESULT_CODE_SUCCESS;
}

const char* hal_get_project_name(void)
{
    return PROJECT_NAME;
}

uint16_t hal_get_fw_version(void)
{
    if(hal_get_led_behavior() == LED_BEHAVIOR_1) {
        mid_set_erp_wakeup_led_blink(false);
        hal_set_erp_led_behavior(false);
    }
    else if(hal_get_led_behavior() == LED_BEHAVIOR_2) {
        mid_set_erp_wakeup_led_blink(false);
        hal_set_erp_led_behavior(true);
    }
    return (MCU_FIRMWARE_VERSION_MAJOR << 8) | MCU_FIRMWARE_VERSION_MINOR;
}

bool hal_get_recovery_state(void)
{
    bool recovered = hal_flash_get_soc_recovery_flag();
    if(recovered) {
        hal_flash_clear_soc_recovery_flag();
    }
    return recovered;
}

void hal_soc_cmd_send(uint8_t *data, uint16_t len)
{
    hal_uart_send(UART_ID_0, data, len);
}

void hal_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len)
{
    if(hal_get_power_mode() != POWER_MODE_ERP) {
        hal_uart_send(UART_ID_1, data, len);
    }
}