#include <stdio.h>
#include "hal_config.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "hal_soc_cmd_interface.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "mid_erp_flow.h"
#include "mid_soc_cmd.h"

uint16_t hal_set_fan_status(uint8_t status)
{
    if(status > 100) {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    else if(status > 9) {
        hal_pwm_set_pulse(PWM_PD13, status);
        hal_pwm_set_enable(PWM_PD13, true);
    }
    else {
        hal_pwm_set_pulse(PWM_PD13, 0);
        hal_pwm_set_enable(PWM_PD13, false);
    }
    return CMD_RESULT_CODE_SUCCESS;
}

#define POWER_DISABLE       ((uint8_t)0x00)
#define POWER_ENABLE        ((uint8_t)0x01)
#define POWER_EXT12V_ONLY   ((uint8_t)0x02)

uint16_t hal_set_ext_12v_power(uint8_t status)
{
    if(status == POWER_DISABLE) {
        hal_gpio_set(EXT_EN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
        hal_gpio_set(MCB_EN_GPIO, GPIO_HIGH);
        hal_gpio_set(EXT12V_EN_GPIO, GPIO_LOW);
    }
    else if(status == POWER_ENABLE) {
        hal_gpio_set(EXT12V_EN_GPIO, GPIO_HIGH);
        hal_gpio_set(MCB_EN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
        hal_gpio_set(EXT_EN_GPIO, GPIO_HIGH);
    }
    else if(status == POWER_EXT12V_ONLY) {
        hal_gpio_set(EXT_EN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
        hal_gpio_set(MCB_EN_GPIO, GPIO_HIGH);
        hal_gpio_set(EXT12V_EN_GPIO, GPIO_HIGH);
    }
    else {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }
    return CMD_RESULT_CODE_SUCCESS;
}

/* IWDG, carried over from the legacy PowerCtrl.c SetIntoMCUWatchdog():
 * LSI (~32kHz) / 64 = 500Hz, reload 4095 -> 4096/500 = ~8.19s before the
 * watchdog fires. hal_reboot_process() refreshes it every 50ms, so there is
 * ample margin as long as the scheduler keeps running. */
static IWDG_HandleTypeDef iwdg_handle;
static bool mcu_watchdog_started = false;

uint16_t hal_set_mcu_watchdog(bool status)
{
    /* The IWDG has no stop bit: once running it runs until reset. The legacy
     * behaved the same way - SetIntoMCUWatchdog() simply did nothing when the
     * SoC asked for a disable - so accept and ignore rather than report a
     * failure the SoC cannot act on. */
    if(!status || mcu_watchdog_started) {
        return CMD_RESULT_CODE_SUCCESS;
    }

    iwdg_handle.Instance       = IWDG;
    iwdg_handle.Init.Prescaler = IWDG_PRESCALER_64;
    iwdg_handle.Init.Reload    = 4095;
    if(HAL_IWDG_Init(&iwdg_handle) != HAL_OK) {
        return CMD_RESULT_CODE_FAILURE;
    }

    mcu_watchdog_started = true;
    return CMD_RESULT_CODE_SUCCESS;
}

void hal_mcu_watchdog_refresh(void)
{
    if(mcu_watchdog_started) {
        HAL_IWDG_Refresh(&iwdg_handle);
    }
}

/* Mirrors the reset sequence the legacy RebootProcess() used for both the
 * reboot timer and the SoC watchdog: assert ERP_EN to drop the SoC rail, give
 * it 100ms to settle, then reset the MCU. */
void hal_set_soc_mcu_reset(void)
{
    hal_gpio_set(ERP_EN_GPIO, GPIO_HIGH);
    hal_delay_1ms(100);
    HAL_DeInit();
    HAL_NVIC_SystemReset();
}

const char* hal_get_project_name(void)
{
    return PROJECT_NAME;
}

uint16_t hal_get_fw_version(void)
{
    /*if(hal_get_led_behavior() == LED_BEHAVIOR_1) {
        mid_set_erp_wakeup_led_blink(false);
        hal_set_erp_led_behavior(false);
    }
    else if(hal_get_led_behavior() == LED_BEHAVIOR_2) {
        mid_set_erp_wakeup_led_blink(false);
        hal_set_erp_led_behavior(true);
    }*/
    return (MCU_FIRMWARE_VERSION_MAJOR << 8) | MCU_FIRMWARE_VERSION_MINOR;
}

void hal_soc_cmd_send(uint8_t *data, uint16_t len)
{
    hal_uart_send(UART_ID_1, data, len);
}