#include "hal_can.h"
#include "hal_gpio.h"
#include "hal_power.h"
#include "hal_system.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "mid_comm_monitor_led.h"
#include "mid_log.h"
#include "mid_tca8418_keypad.h"

#ifdef TONE_CONTROL_BY_PWM
#include "hal_pwm.h"
#endif

uint8_t power_mode = POWER_MODE_ACTIVE;
uint8_t hal_get_power_mode(void) 
{
    return power_mode;
}

void hal_set_power_mode(uint8_t mode) 
{
    power_mode = mode;
}

void hal_set_soc_power_on(void)
{
    hal_gpio_set(MCU_EUPS_GPIO, GPIO_HIGH);
    hal_gpio_set(SOC_5VS5_PEN_GPIO, GPIO_HIGH);
}

void hal_set_soc_power_off(void)
{
    hal_gpio_set(SOC_5VS5_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_EUPS_GPIO, GPIO_LOW);
}

void hal_set_erp_sleep(void)
{
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
}

bool erp_wakeup_flag = false;
void hal_set_erp_wakeup_flag(void)
{
    erp_wakeup_flag = true;
}

void hal_set_erp_power_off(void)
{
    hal_gpio_module_deinit();

#ifdef TONE_CONTROL_BY_PWM
    hal_pwm_deinit();
#endif
    hal_can_deinit();
    hal_uart_deinit();

    mid_enable_soc_lcb_comm_timer(false);
    hal_set_power_mode(POWER_MODE_ERP);
    erp_wakeup_flag = false;
    uint16_t press_ms = 0;
    uint16_t release_ms = 0;
    while(!erp_wakeup_flag) {
        hal_set_erp_sleep();
        release_ms = 0;
        while(hal_gpio_get(ERP_IR_INT_GPIO) == GPIO_LOW) {
            hal_delay_1ms(10);
            press_ms += 10;
            if (press_ms >= 2000) {
                goto WAKEUP_SUCCESS;
            }
        }
        if (press_ms > 0) {
            while(hal_gpio_get(ERP_IR_INT_GPIO) == GPIO_HIGH) {
                hal_delay_1ms(10);
                release_ms += 10;
                if (release_ms >= 100) {
                    press_ms = 0;
                    break;
                }
            }
        }
    }

WAKEUP_SUCCESS:
    hal_input_gpio_deinit(MCU_KEY_INT_GPIO);
    hal_system_init();
    hal_set_power_mode(POWER_MODE_ACTIVE);
}

void hal_set_soc_mcu_reset(void)
{
    hal_gpio_set(MCU_RESET_KEY_GPIO, GPIO_HIGH);
    NVIC_SystemReset();
}