#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_sysclk.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "hal_usb.h"
#include "hal_pwm.h"
#include "mid_erp_flow.h"
#include "stm32f4xx.h"
#include "hal_flash.h"

void hal_system_init()
{
    hal_sysclk_config();
    hal_systick_config();
    hal_gpio_module_init();
    hal_usb_init();
    hal_adc_init();
    hal_uart_init();
    hal_pwm_init();
    hal_flash_init();

    mid_erp_status_set(ENTER_ERP_READY_STATUS);
}

unsigned long SvcProcess(unsigned int *pSvcDta)
{
    return 0;
}