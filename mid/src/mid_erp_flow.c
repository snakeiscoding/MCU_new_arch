#include "app_soc_cmd.h"
#include "hal_lcb_cmd_interface.h"
#include "hal_soc_cmd_interface.h"
#include "mid_erp_flow.h"
#include "mid_lcb_cmd.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

ERP_STATUS erp_status = ENTER_ERP_READY_STATUS;
static uint16_t erp_startup_delay_ms = 0;
static uint16_t erp_lcb_ready_delay_ms = 0;
static bool erp_lcb_woken = false;
static uint16_t erp_timer = 0;
static uint8_t start_erp_timer_cnt = 0;
static bool start_erp_timer = false;
static uint16_t erp_led_blink_timer = 0;
static bool start_erp_led_blink = false;

uint8_t mid_erp_status_get(void)
{
    return erp_status;
}

void mid_erp_status_set(ERP_STATUS status)
{
    erp_status = status;
    erp_timer = 0;
    start_erp_timer_cnt = 0;
    start_erp_timer = false;
    erp_lcb_woken = false;
}

/* How long the LCB needs to boot after its rail returns, before the wake
 * pulse can reach it. */
void mid_set_erp_startup_delay_ms(uint16_t delay_ms)
{
    erp_startup_delay_ms = delay_ms;
}

/* How long to leave the LCB after the wake pulse before the first command. */
void mid_set_erp_lcb_ready_delay_ms(uint16_t delay_ms)
{
    erp_lcb_ready_delay_ms = delay_ms;
}

void mid_set_erp_wakeup_led_blink(bool enable)
{
    erp_led_blink_timer = 0;
    start_erp_led_blink = enable;
}

#define LCB_NO_RESPONSE     ((uint8_t)0x00)
#define LCB_ENTER_ERP_MODE  ((uint8_t)0x01)
#define LCB_NOT_SUPPORT_EUP ((uint8_t)0x02)
#define LCB_LEAVE_ERP_MODE  ((uint8_t)0x03)

__attribute__((weak)) void hal_lcb_leave_erp_success_handler(void) {}
__attribute__((weak)) void hal_lcb_erp_cmd_fail_handler(void) {}
__attribute__((weak)) void hal_set_erp_soc_power_off(void) {}
__attribute__((weak)) void hal_set_erp_soc_power_off_success_handler(void) {}
__attribute__((weak)) void hal_set_erp_led_behavior(bool status) {}
void mid_erp_status_handler(void)
{
    if(start_erp_led_blink) {
        erp_led_blink_timer++;
        if(erp_led_blink_timer == 1000) {
            hal_set_erp_led_behavior(true);
            erp_led_blink_timer = 0;
        }
        else if(erp_led_blink_timer == 500) {
            hal_set_erp_led_behavior(false);
        }
    }
    if(erp_status == LEAVE_ERP_READY_STATUS) {
        return;
    }
    switch(erp_status) {
    case ENTER_ERP_READY_STATUS:
        if(!start_erp_timer) {
            // mid_log_print(LOG_VERBOSE, "LCB init\r\n");
            // mid_lcb_wakeup_init();
            start_erp_timer_cnt = 0;
            start_erp_timer = true;
        }
        erp_timer++;
        /*Ensure LCB rail is stable before communication*/
        // if(erp_timer > erp_startup_delay_ms) {
        /* Phase 1: wait for the LCB to boot before waking it.
         *
         * Leaving ERP cuts the LCB's rail, and it comes back with the wake -
         * measured back at 12.4V only 19ms in - so the pulse used to be sent
         * while the LCB was still starting up, and it never arrived. Measured
         * on the bench across three sleep/wake cycles: a pulse at t=0.4s left
         * the LCB silent every time, one at t=2.7s woke it every time, and it
         * then answered the first command within 30ms. */
        if(!erp_lcb_woken) {
            if(erp_timer > erp_startup_delay_ms) {
                mid_log_print(LOG_VERBOSE, "LCB init\r\n");
                mid_lcb_wakeup_init();
                erp_lcb_woken = true;
                erp_timer = 0;
            }
            break;
        }
        /* Phase 2: settle after the pulse before the first command. */
        if(erp_timer > erp_lcb_ready_delay_ms) {
            mid_erp_status_set(LCB_WAKE_UP_STATUS);
        }
        break;
    case LCB_WAKE_UP_STATUS:
        if(!start_erp_timer) {
            mid_lcb_send_erp_cmd(LCB_LEAVE_ERP);
            erp_timer = 0;
            start_erp_timer = true;
        }
        erp_timer++;
        if(erp_timer > 300) {
            erp_timer = 0;
            start_erp_timer_cnt++;
            start_erp_timer = false;
        }

        if(start_erp_timer_cnt > 2) {
            mid_log_print(LOG_ERROR, "More than 3 times no reply from LCB(LCB_WAKE_UP_STATUS)\r\n");
            mid_erp_status_set(LEAVE_ERP_READY_STATUS);
            hal_lcb_leave_erp_success_handler();
        }
        break;
    case LCB_ENTER_ERP_STATUS:
        if(!start_erp_timer) {
            mid_lcb_send_erp_cmd(LCB_ENTER_ERP);
            erp_timer = 0;
            start_erp_timer = true;
        }
        erp_timer++;
        if(erp_timer > 300) {
            erp_timer = 0;
            start_erp_timer_cnt++;
            start_erp_timer = false;
        }

        if(start_erp_timer_cnt > 2) {
            mid_log_print(LOG_ERROR, "More than 3 times no reply from LCB(LCB_ENTER_ERP_STATUS)\r\nReboot MCU and SoC\r\n");
            hal_lcb_erp_cmd_fail_handler();
        }
        break;
    case LCB_INIT_STATUS:
        if(!start_erp_timer) {
            /* Note: Run 'lcb init' for ERP access in non-GUI mode */
            mid_lcb_send_init_cmd();
            erp_timer = 0;
            start_erp_timer = true;
        }
        erp_timer++;
        if(erp_timer > 300) {
            erp_timer = 0;
            start_erp_timer_cnt++;
            start_erp_timer = false;
        }

        if(start_erp_timer_cnt > 2) {
            mid_log_print(LOG_ERROR, "Init More than 3 times no reply from LCB(LCB_INIT_STATUS)\r\n");
            mid_erp_status_set(LCB_ENTER_ERP_STATUS);
        }
        break;
    case SOC_POWER_OFF_STATUS: {
        if(!start_erp_timer) {
            uint16_t data_len = 1;
            uint8_t data[1];
            data[0] = LCB_ENTER_ERP_MODE;
            mid_soc_cmd_send(CMD_EVENT_LCB_STATUS, CMD_RESULT_CODE_SUCCESS, data, data_len);
            erp_timer = 0;
            start_erp_timer = true;
        }
        erp_timer++;
        if(erp_timer == 3000) {
            hal_set_erp_soc_power_off();
        }
        else if(erp_timer > 4000) {
            hal_set_erp_soc_power_off_success_handler();
        }
        break;
    }
    default:
        break;
    }
}