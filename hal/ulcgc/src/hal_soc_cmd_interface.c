#include <stdio.h>
#include "hal_config.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_power.h"
#include "hal_soc_cmd_interface.h"
#include "hal_timer_handler.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "mid_comm_monitor_led.h"
#include "mid_erp_flow.h"
#include "mid_soc_cmd.h"

#ifdef TONE_CONTROL_BY_PWM
#include "hal_pwm.h"
#endif

uint8_t hal_get_estop_status(void)
{
    return hal_gpio_get(MCU_SW_POSITION_GPIO);
}

#define TOGGLE_TONE ((uint8_t)0x00)
#define LOW_TONE    ((uint8_t)0x01)
#define MID_TONE    ((uint8_t)0x02)
#define HIGH_TONE   ((uint8_t)0x03)

#define BUZZER_LOW_TONE  ((uint32_t) 100)
#define BUZZER_MID_TONE  ((uint32_t) 150)
#define BUZZER_HIGH_TONE ((uint32_t) 250)

bool buzzer_status_on = true;
void hal_control_buzzer(uint8_t status, bool power_on)
{
    if(!power_on) {
#ifdef TONE_CONTROL_BY_PWM
        hal_pwm_set_enable(PWM_PA11, false);
#else
        hal_gpio_set(MCU_BEEP_GPIO, GPIO_LOW);
#endif
    } 
    else {
        uint32_t time = 0;
        if(status == LOW_TONE) {
            time = BUZZER_LOW_TONE;
        }
        else if(status == MID_TONE) {
            time = BUZZER_MID_TONE;
        }
        else if(status == HIGH_TONE) {
            time = BUZZER_HIGH_TONE;
        }
#ifdef TONE_CONTROL_BY_PWM
        hal_pwm_set_pulse(PWM_PA11, pulse);
        hal_pwm_set_enable(PWM_PA11, true);
#else
        hal_gpio_set(MCU_BEEP_GPIO, GPIO_HIGH);
#endif
		hal_set_buzzer_start_timer(time);
    }
}
uint16_t hal_set_tone_status(uint8_t status)
{
    if(status == TOGGLE_TONE) {
        buzzer_status_on = !buzzer_status_on;
        hal_control_buzzer(status, false);
    }
    else if(status == LOW_TONE && buzzer_status_on) {
        hal_control_buzzer(LOW_TONE, true);
    }
    else if(status == MID_TONE && buzzer_status_on) {
        hal_control_buzzer(MID_TONE, true);
    }
    else if(status == HIGH_TONE && buzzer_status_on) {
        hal_control_buzzer(HIGH_TONE, true);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
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
        hal_set_current_monitor(true);
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

uint16_t hal_set_qi_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(QI_9VPEN_GPIO, GPIO_LOW);
        hal_gpio_set(QI_12VPEN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_set_current_monitor(true);
        return hal_set_qi_status(hal_get_qi_status());
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
        hal_gpio_set(RS232_5V_EN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(RS232_5V_EN_GPIO, GPIO_HIGH);
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
uint8_t qi_status = QI_9V;
uint8_t hal_get_qi_status()
{
    return qi_status;
}
uint16_t hal_set_qi_status(uint8_t status)
{
    if(status == QI_9V) {
        qi_status = status;
        hal_gpio_set(QI_12VPEN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
        hal_gpio_set(QI_9VPEN_GPIO, GPIO_HIGH);
    }
    else if(status == QI_12V) {
        qi_status = status;
        hal_gpio_set(QI_9VPEN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
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