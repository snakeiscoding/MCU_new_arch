#include "hal_adc.h"
#include "hal_can.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_power.h"
#include "hal_system.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "hal_usb.h"
#include "mid_erp_flow.h"
#include "mid_tca8418_keypad.h"

#ifdef TONE_CONTROL_BY_PWM
#include "hal_pwm.h"
#endif

void hal_system_init()
{
    hal_systick_config();
    hal_usb_init();
    hal_adc_init();
    hal_gpio_module_init();
    hal_uart_init();
    hal_can_init();
    hal_i2c_init();
#ifdef TONE_CONTROL_BY_PWM
    hal_pwm_init();
#endif
    hal_flash_init();

    mid_tca8418_reset_keypad_init();
    mid_set_erp_startup_delay_ms(5000);
    mid_erp_status_set(ENTER_ERP_READY_STATUS);
    if(hal_get_power_mode() == POWER_MODE_ERP) {
        mid_set_erp_wakeup_led_blink(true);
    }
}