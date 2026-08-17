#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd_watchdog_func.h"
#include "hal_soc_cmd_interface.h"
#include "hal_timer_handler.h"
#include "mid_erp_flow.h"
#include "mid_log.h"

/* The legacy firmware grouped the reboot timer, the IWDG refresh and the SoC
 * watchdog into a single RebootProcess(), scheduled on TIM9 with a 50-tick
 * divider. TIM9 ticked at 1ms, so that is a 50ms period. This tick is also
 * 1ms, so the same divider reproduces the legacy rate. */
#define REBOOT_PROCESS_PERIOD_MS ((uint32_t)50)

static uint32_t reboot_process_tick = 0;

/* Same three checks, in the same order, as the legacy PowerCtrl.c
 * RebootProcess(). The timing state itself lives in the app layer
 * (app_soc_cmd_control_func.c / app_soc_cmd_watchdog_func.c); this only
 * supplies the periodic call those two check functions were missing. */
static void hal_reboot_process(void)
{
    if(app_soc_cmd_control_reboot_check()) {
        mid_log_print(LOG_INFO, "reboot timeout, resetting SoC and MCU\r\n");
        hal_set_soc_mcu_reset();
    }

    if(app_soc_cmd_control_get_mcu_watchdog_status()) {
        hal_mcu_watchdog_refresh();
    }

    if(app_soc_cmd_watchdog_check()) {
        mid_log_print(LOG_INFO, "soc watchdog timeout, resetting SoC and MCU\r\n");
        hal_set_soc_mcu_reset();
    }
}

void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size)
{
    mid_erp_status_handler();
    
    if(++reboot_process_tick >= REBOOT_PROCESS_PERIOD_MS) {
        reboot_process_tick = 0;
        hal_reboot_process();
    }
    /*mid_tca8418_keypad_read_handler();*/
}