/*
 * hal_key_handler.c - chimera3 / STM32F407ZET6
 *
 * Matrix keypad and safety key. Both are plain GPIO and both are polled from
 * the 1ms tick; neither uses an interrupt.
 *
 * Ported from the legacy MatrixKey.c and DirButton.c, with the blocking waits
 * those used replaced by state that advances one step per tick - the legacy
 * spent 24ms inside HAL_Delay() per keypad scan and another 10ms inside the
 * safety key's EXTI handler, which this architecture cannot afford in the
 * single cooperative main loop.
 */

#include <stdbool.h>
#include "app_soc_cmd.h"
#include "app_soc_cmd_event_func.h"
#include "hal_gpio.h"
#include "hal_key_handler.h"
#include "hal_soc_cmd_interface.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

extern volatile uint64_t time_1ms;

/* ------------------------------------------------------------------------
 * Matrix keypad
 * ------------------------------------------------------------------------ */

#define KEY_DRIVE_NUM   6
#define KEY_SENSE_NUM   2
#define KEY_NUM         (KEY_DRIVE_NUM * KEY_SENSE_NUM)

/* Wire values, matching the legacy KeyPush / KeyPull. */
#define KEY_STATE_DOWN  ((uint8_t)0x00)
#define KEY_STATE_UP    ((uint8_t)0x01)

static const output_gpio_list_num key_drive_gpio[KEY_DRIVE_NUM] = {
    KEYH_D1_GPIO, KEYH_D2_GPIO, KEYH_D3_GPIO, KEYH_D4_GPIO, KEYH_D5_GPIO, KEYH_D6_GPIO
};

static const input_gpio_list_num key_sense_gpio[KEY_SENSE_NUM] = {
    KEYV_D1_GPIO, KEYV_D2_GPIO
};

/* One bit per key, laid out as (sense * KEY_DRIVE_NUM + drive) so the bit order
 * matches the legacy KeyScanTable. Row and column are derived rather than kept
 * in a table: the legacy KeyDtaMapTab is exactly (1 << drive, 1 << sense). */
static uint16_t key_scan_sample = 0;    /* accumulating during the current scan */
static uint16_t key_prev_sample = 0;    /* the previous completed scan */
static uint16_t key_stable = 0;         /* last state reported to the SoC */

static uint8_t key_drive_index = 0;
static bool key_settling = false;

static void hal_key_send_event(uint8_t state, uint8_t drive, uint8_t sense)
{
    uint8_t data[3];

    data[0] = state;
    data[1] = (uint8_t)(1u << drive);    /* row */
    data[2] = (uint8_t)(1u << sense);    /* column */

    mid_log_print(LOG_INFO, "key %s row 0x%02X col 0x%02X\r\n",
                  (state == KEY_STATE_DOWN) ? "down" : "up", data[1], data[2]);
    mid_soc_cmd_send(CMD_EVENT_KEY, CMD_RESULT_CODE_SUCCESS, data, 3);
}

/* The legacy decided push vs pull by comparing the whole bitmap
 * (KeyStatus > LastKeyTemp), which misreports when one key goes down while
 * another comes up in the same scan. Each changed bit is classified on its own
 * new level here instead. */
static void hal_key_report_changes(uint16_t changed, uint16_t current)
{
    bool any_pressed = false;

    for(uint8_t bit = 0; bit < KEY_NUM; bit++) {
        if((changed & (1u << bit)) == 0) {
            continue;
        }
        if(current & (1u << bit)) {
            hal_key_send_event(KEY_STATE_DOWN, bit % KEY_DRIVE_NUM, bit / KEY_DRIVE_NUM);
            any_pressed = true;
        }
        else {
            hal_key_send_event(KEY_STATE_UP, bit % KEY_DRIVE_NUM, bit / KEY_DRIVE_NUM);
        }
    }

    /* The legacy beeped once per scan that contained a press, not once per key. */
    if(any_pressed) {
        hal_play_key_tone();
    }
}

/* Two ticks per drive line: one to let the line and the sense inputs settle
 * (the legacy's HAL_Delay(1)), one to sample. A full 6-line scan therefore
 * takes 12ms. Both sense lines are read in the same step, so unlike the legacy
 * each drive line is only pulsed once per scan instead of twice. */
static void hal_key_scan_step(void)
{
    if(!key_settling) {
        hal_gpio_set(key_drive_gpio[key_drive_index], GPIO_LOW);
        key_settling = true;
        return;
    }

    for(uint8_t sense = 0; sense < KEY_SENSE_NUM; sense++) {
        if(hal_gpio_get(key_sense_gpio[sense]) == GPIO_LOW) {
            key_scan_sample |= (uint16_t)(1u << (sense * KEY_DRIVE_NUM + key_drive_index));
        }
    }
    hal_gpio_set(key_drive_gpio[key_drive_index], GPIO_HIGH);
    key_settling = false;
    key_drive_index++;

    if(key_drive_index < KEY_DRIVE_NUM) {
        return;
    }
    key_drive_index = 0;

    /* A change is only acted on once two consecutive scans agree, which gives
     * roughly 24ms of debounce. The legacy had none beyond its slow 150ms scan
     * period; this scan is faster, so the filtering has to be explicit. */
    if(key_scan_sample == key_prev_sample && key_scan_sample != key_stable) {
        hal_key_report_changes(key_scan_sample ^ key_stable, key_scan_sample);
        key_stable = key_scan_sample;
    }
    key_prev_sample = key_scan_sample;
    key_scan_sample = 0;
}

/* ------------------------------------------------------------------------
 * Safety key (E-Stop)
 * ------------------------------------------------------------------------ */

/* Same 10ms the legacy DirButton.c waited before re-reading the pin. */
#define SAFETY_KEY_DEBOUNCE_MS  ((uint64_t)10)

static gpio_status safety_key_stable = GPIO_HIGH;
static gpio_status safety_key_sample = GPIO_HIGH;
static uint64_t safety_key_sample_time = 0;
static bool safety_key_ready = false;

uint8_t hal_get_estop_status(void)
{
    /* Raw pin level. app_soc_cmd_event_func.c maps 1 to ESTOP_CLEAR and 0 to
     * ESTOP_TRIGGER, which is the same polarity the legacy GetSafetyKeyStatus()
     * used. */
    return (hal_gpio_get(SAFETY_KEY_GPIO) == GPIO_LOW) ? 0u : 1u;
}

static void hal_safety_key_step(void)
{
    gpio_status level = hal_gpio_get(SAFETY_KEY_GPIO);

    /* The very first sample establishes the baseline. Latching it here rather
     * than waiting for a transition matters: if the key already sits in the
     * state the variables were initialised to, a transition-based baseline
     * would swallow the first genuine change. */
    if(!safety_key_ready) {
        safety_key_ready = true;
        safety_key_stable = level;
        safety_key_sample = level;
        safety_key_sample_time = time_1ms;
        return;
    }

    if(level != safety_key_sample) {
        safety_key_sample = level;
        safety_key_sample_time = time_1ms;
        return;
    }

    if(level == safety_key_stable) {
        return;
    }
    if((time_1ms - safety_key_sample_time) < SAFETY_KEY_DEBOUNCE_MS) {
        return;
    }

    safety_key_stable = level;

    mid_log_print(LOG_INFO, "safety key %s\r\n",
                  (level == GPIO_LOW) ? "triggered" : "cleared");
    app_soc_cmd_event_estop_func();
}

void hal_key_handler_1ms(void)
{
    hal_key_scan_step();
    hal_safety_key_step();
}