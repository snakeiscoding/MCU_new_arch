#include <stdio.h>
#include "hal_lcb_cmd_interface.h"
#include "mid_comm_monitor_led.h"
#include "mid_lcb_cmd.h"
#include "mid_log.h"

extern volatile uint64_t time_1ms;

uint64_t soc_mcu_comm_timer = 0;
LED_COMM_STATUS soc_mcu_comm_led_status = LED_IDLE_STATUS;
uint64_t soc_lcb_comm_timer = 0;
uint64_t soc_lcb_comm_timeout_timer = 0;
LED_COMM_STATUS soc_lcb_comm_led_status = LED_IDLE_STATUS;
LED_COMM_STATUS soc_lcb_comm_timeout_led_status = LED_IDLE_STATUS;
bool start_soc_lcb_comm_timer_enable = false;

LED_COMM_STATUS mid_get_soc_mcu_comm_led_status(void)
{
    if(soc_mcu_comm_led_status == LED_IDLE_STATUS) {
        return LED_IDLE_STATUS;
    }

    uint64_t elapsed_time = time_1ms - soc_mcu_comm_timer;
    if(elapsed_time > 0 && elapsed_time < 2001) {
        if(soc_mcu_comm_led_status != LED_ON_STATUS) {
            soc_mcu_comm_led_status = LED_ON_STATUS;
            return LED_ON_STATUS;
        }
    }
    else if(elapsed_time > 2000 && elapsed_time < 2501) {
        if(soc_mcu_comm_led_status != LED_OFF_STATUS) {
            soc_mcu_comm_led_status = LED_OFF_STATUS;
            return LED_OFF_STATUS;
        }
    }
    else if(elapsed_time > 2500) {
        soc_mcu_comm_led_status = LED_IDLE_STATUS;
        return LED_IDLE_STATUS;
    }
    return LED_NO_CHANGE_STATUS;
}

void mid_set_soc_mcu_comm_timer(void)
{
    if(soc_mcu_comm_led_status == LED_IDLE_STATUS) {
        soc_mcu_comm_timer = time_1ms;
        soc_mcu_comm_led_status = LED_OFF_STATUS;
    }
}

LED_COMM_STATUS mid_get_soc_lcb_comm_led_status(void)
{
    if(!start_soc_lcb_comm_timer_enable) {
        return LED_OFF_STATUS;
    }
    if(soc_lcb_comm_led_status == LED_IDLE_STATUS) {
        return LED_IDLE_STATUS;
    }

    uint64_t elapsed_time = time_1ms - soc_lcb_comm_timer;
    if(elapsed_time > 0 && elapsed_time < 2001) {
        if(soc_lcb_comm_led_status != LED_ON_STATUS) {
            soc_lcb_comm_led_status = LED_ON_STATUS;
            return LED_ON_STATUS;
        }
    }
    else if(elapsed_time > 2000 && elapsed_time < 2501) {
        if(soc_lcb_comm_led_status != LED_OFF_STATUS) {
            soc_lcb_comm_led_status = LED_OFF_STATUS;
            return LED_OFF_STATUS;
        }
    }
    else if(elapsed_time > 2500) {
        soc_lcb_comm_led_status = LED_IDLE_STATUS;
        return LED_IDLE_STATUS;
    }
    return LED_NO_CHANGE_STATUS;
}

uint8_t last_cmd_buffer[256];
uint16_t last_cmd_len = 0;
SOC_LCB_COMM_STATUS soc_lcb_comm_status = NORMAL_COMM_STATUS;
__attribute__((weak)) uint16_t hal_get_soc_lcb_last_cmd(uint8_t *cmd) {return 0;}
LED_COMM_STATUS mid_get_soc_lcb_comm_timeout_status(void)
{
    if(!start_soc_lcb_comm_timer_enable) {
        return LED_OFF_STATUS;
    }
    switch((SOC_LCB_COMM_STATUS)soc_lcb_comm_status) {
        case NORMAL_COMM_STATUS: {
            uint64_t elapsed_time = time_1ms - soc_lcb_comm_timer;
            if(elapsed_time > 7500) {
                soc_lcb_comm_status = SOC_LCB_COMM_TIMEOUT_STATUS;
            }
            break;
        }
        case SOC_LCB_COMM_TIMEOUT_STATUS:
            last_cmd_len = hal_get_soc_lcb_last_cmd(last_cmd_buffer);
            mid_log_print(LOG_INFO, "SoC and LCB timeout last cmd : ");
            for (int i = 0; i < last_cmd_len; i++) {
                printf("%02X ", last_cmd_buffer[i]);
            }
            printf("\r\n");
            mid_lcb_send_version_cmd();
            soc_lcb_comm_status = MCU_LCB_COMM_TEST_STATUS;
            break;
        case MCU_LCB_COMM_TEST_STATUS:
            soc_lcb_comm_timeout_timer++;
            if(soc_lcb_comm_timeout_timer > 1001) {
                soc_lcb_comm_timeout_timer = 0;
                soc_lcb_comm_status = SOC_LCB_COMM_FAIL_STATUS;
            }
            break;
        case SOC_COMM_FAIL_STATUS:
            soc_lcb_comm_timeout_timer++;
            if(soc_lcb_comm_timeout_timer > 0 && soc_lcb_comm_timeout_timer < 501) {
                if(soc_lcb_comm_timeout_led_status != LED_ON_STATUS) {
                    soc_lcb_comm_timeout_led_status = LED_ON_STATUS;
                    return LED_ON_STATUS;
                }
            }
            else if(soc_lcb_comm_timeout_timer > 500 && soc_lcb_comm_timeout_timer < 1001) {
                if(soc_lcb_comm_timeout_led_status != LED_OFF_STATUS) {
                    soc_lcb_comm_timeout_led_status = LED_OFF_STATUS;
                    return LED_OFF_STATUS;
                }
            }
            else if(soc_lcb_comm_timeout_timer > 1000) {
                soc_lcb_comm_timeout_timer = 0;
            }
            break;
        case SOC_LCB_COMM_FAIL_STATUS:
            if(soc_lcb_comm_timeout_led_status != LED_OFF_STATUS) {
                soc_lcb_comm_timeout_led_status = LED_OFF_STATUS;
                return LED_OFF_STATUS;
            }
            break;
    }
    return LED_NO_CHANGE_STATUS;
}

void mid_set_soc_lcb_comm_timer(void)
{
    if(!start_soc_lcb_comm_timer_enable) {
        return;
    }
    if(soc_lcb_comm_status == MCU_LCB_COMM_TEST_STATUS) {
        return;
    }
    if(soc_lcb_comm_led_status == LED_IDLE_STATUS) {
        soc_lcb_comm_timer = time_1ms;
        soc_lcb_comm_led_status = LED_OFF_STATUS;
        soc_lcb_comm_timeout_timer = 0;
        soc_lcb_comm_timeout_led_status = LED_OFF_STATUS;
        soc_lcb_comm_status = NORMAL_COMM_STATUS;
    }
}

void mid_enable_soc_lcb_comm_timer(bool enable)
{
    start_soc_lcb_comm_timer_enable = enable;
}

SOC_LCB_COMM_STATUS mid_get_mcu_lcb_handshake_status(void)
{
    return soc_lcb_comm_status;
}

void mid_set_mcu_lcb_handshake_success(void)
{
    soc_lcb_comm_status = SOC_COMM_FAIL_STATUS;
    soc_lcb_comm_timeout_timer = 0;
}