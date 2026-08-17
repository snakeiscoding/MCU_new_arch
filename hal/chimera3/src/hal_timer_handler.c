#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd_watchdog_func.h"
#include "hal_soc_cmd_interface.h"
#include "hal_timer_handler.h"
#include "mid_erp_flow.h"
#include "mid_log.h"

/* The legacy firmware grouped the reboot timer, the IWDG refresh and the SoC
 * watchdog into a single RebootProcess(), scheduled on TIM9 with a 50-tick
 * divider. TIM9 ticked at 1ms, so that is a 50ms period.
 *
 * Paced against time_1ms rather than by counting how often this handler runs.
 * Counting would in fact be adequate here - the checks inside all compare
 * time_1ms themselves, so the divider only sets how often they are asked, and
 * a burst of queued ticks changes the spacing but not the long-run rate. It is
 * written this way so that every interval in this file is measured the same
 * way, because the one place where counting is NOT adequate (the buzzer pulse
 * below) already shipped broken once by looking exactly like this. */
#define REBOOT_PROCESS_PERIOD_MS ((uint64_t)50)

static uint64_t reboot_process_last_time = 0;

/* Buzzer release. The legacy held the tone with a blocking HAL_Delay(20)
 * inside BuzzerTrigerPrecess(); doing that here would stall the scheduler for
 * 20ms on every beep, so the pulse is timed off this 1ms tick instead. 
 *
 * The elapsed time is measured against time_1ms rather than by counting how
 * often this handler runs. Counting invocations does not measure milliseconds:
 * SysTick keeps queueing tick events while the main loop is busy (the debug
 * printf path alone blocks for several ms per line), and the scheduler then
 * drains that backlog back to back. A 20-invocation count can therefore elapse
 * in a handful of real milliseconds - far too short for the buzzer to trigger,
 * which is exactly how this failed the first time. */
#define TONE_ON_TIME_MS ((uint64_t)20)

extern volatile uint64_t time_1ms;

static bool tone_on_running = false;
static uint64_t tone_on_start_time = 0;

/* Re-arming on a fresh trigger (rather than ignoring it while a tone is still
 * playing) means every accepted tone gets its full 20ms, which is what the
 * legacy's blocking delay gave. */
void hal_set_tone_start_timer(void)
{
    tone_on_start_time = time_1ms;
    tone_on_running = true;
}

static void hal_tone_on_handler(void)
{
    if(!tone_on_running) {
        return;
    }
    if((time_1ms - tone_on_start_time) >= TONE_ON_TIME_MS) {
        tone_on_running = false;
        hal_control_tone(0, false);
    }
}

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

    hal_tone_on_handler();
    
    if((time_1ms - reboot_process_last_time) >= REBOOT_PROCESS_PERIOD_MS) {
        reboot_process_last_time = time_1ms;
        hal_reboot_process();
    }
    /*mid_tca8418_keypad_read_handler();*/
}