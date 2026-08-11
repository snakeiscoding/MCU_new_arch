#include <stdbool.h>
#include <stdio.h>
#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd_watchdog_func.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_key_handler.h"
#include "hal_power.h"
#include "hal_soc_cmd_interface.h"
#include "hal_timer_handler.h"
#include "mid_comm_monitor_led.h"
#include "mid_erp_flow.h"
#include "mid_tca8418_keypad.h"

#define SPEAKER_ON_TIME ((uint32_t) 20)
bool start_speaker_on_timer = false;
uint32_t speaker_on_timer = 0;
void hal_set_tone_start_timer(void)
{
    if(!start_speaker_on_timer) {
        start_speaker_on_timer = true;
        speaker_on_timer = 0;
    }
}

void hal_tone_on_handler(void)
{
    if(start_speaker_on_timer) {
        speaker_on_timer++;
        if(speaker_on_timer > SPEAKER_ON_TIME) {
            hal_control_tone(0x00, false);
            start_speaker_on_timer = false;
            speaker_on_timer = 0;
        }
    }
}

bool mcb12v_overcurrent(void)
{
    return ((float)hal_get_MCB12V_current_sample() > 4.5f);
}

typedef enum {
    CURR_MON_IDLE = 0,
    CURR_MON_STAGE1,
    CURR_MON_STAGE2,
} CURR_MON_STATE;
static CURR_MON_STATE curr_state = CURR_MON_IDLE;
uint32_t current_timer = 0;
uint32_t stage_wait_timer = 0;
void hal_current_monitor_handler(void)
{
    if (current_timer % 10 == 0) {
        hal_MCB12V_current_sample();
    }

    switch(curr_state) {
    case CURR_MON_IDLE:
        current_timer++;
        if (current_timer >= 5000) {
            current_timer = 0;
            if (mcb12v_overcurrent()) {
                hal_gpio_set(QI_12VPEN_GPIO, GPIO_LOW);
                curr_state = CURR_MON_STAGE1;
                stage_wait_timer = 0;
            }
        }
        break;
    case CURR_MON_STAGE1:
        stage_wait_timer++;
        if (stage_wait_timer >= 100) {
            stage_wait_timer = 0;
            if (mcb12v_overcurrent()) {
                hal_gpio_set(CAN_12V_EN_GPIO, GPIO_LOW);
                curr_state = CURR_MON_STAGE2;
            }
            else {
                curr_state = CURR_MON_IDLE;
            }
        }
        break;
    case CURR_MON_STAGE2:
        stage_wait_timer++;
        if (stage_wait_timer >= 100) {
            stage_wait_timer = 0;
            if (mcb12v_overcurrent()) {
                hal_gpio_set(MCU_CSAFE_PEN_GPIO, GPIO_LOW);
            }
            curr_state = CURR_MON_IDLE;
        }
        break;
    }
}

void hal_reboot_handler(void)
{
    if(app_soc_cmd_control_reboot_check()) {
        hal_set_soc_mcu_reset();
    }
}

void hal_mcu_watchdog_handler(void)
{
    if(app_soc_cmd_control_get_mcu_watchdog_status()) {
        fwdgt_counter_reload();
    }
}

void hal_soc_watchdog_handler(void)
{
    if(app_soc_cmd_watchdog_check()) {
        hal_set_soc_mcu_reset();
    }
}

void hal_comm_monitor_led_handler(void)
{
    LED_COMM_STATUS led_status = mid_get_soc_lcb_comm_led_status();
    if(led_status == LED_OFF_STATUS) {
        hal_gpio_set(MCU_LED0_GPIO, GPIO_LOW);
    }
    else if(led_status == LED_ON_STATUS) {
        hal_gpio_set(MCU_LED0_GPIO, GPIO_HIGH);
    }

    led_status = mid_get_soc_lcb_comm_timeout_status();
    if(led_status == LED_OFF_STATUS) {
        hal_gpio_set(MCU_LED0_GPIO, GPIO_LOW);
    }
    else if(led_status == LED_ON_STATUS) {
        hal_gpio_set(MCU_LED0_GPIO, GPIO_HIGH);
    }
}

void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size)
{
    mid_tca8418_keypad_init();
    hal_reboot_handler();
    hal_mcu_watchdog_handler();
    hal_soc_watchdog_handler();
    hal_mcu_recovery_key_handler();
    hal_tone_on_handler();
    mid_erp_status_handler();
    hal_comm_monitor_led_handler();
    hal_current_monitor_handler();
    /*mid_tca8418_keypad_read_handler();*/
}