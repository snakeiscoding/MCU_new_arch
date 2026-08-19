#include <stdio.h>
#include "hal_config.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "hal_soc_cmd_interface.h"
#include "hal_systick.h"
#include "hal_timer_handler.h"
#include "hal_uart.h"
#include "mid_erp_flow.h"
#include "mid_soc_cmd.h"

/* AP890XX buzzer.
 *
 * The legacy split this in two: SetTone() only latched enable/type, and the
 * sound was produced later by BuzzerTrigerPrecess() when MatrixKey.c saw a key
 * press. chimera3 has no keypad wired up in this build, so a faithful port
 * would leave the Tone command with no observable effect at all. This follows
 * the vortex2 convention instead - the command itself plays the tone - which
 * keeps 0x0001 verifiable on its own. When the matrix keypad is added later it
 * can call hal_control_tone() on a key press and both paths will beep.
 *
 * The pulse width and the pin pattern are the legacy's: nRST and S4 low, one
 * of S1/S2/S3 high for 20ms, then all low. The legacy used a blocking
 * HAL_Delay(20); here the timer handler releases it instead. */
#define TOGGLE_TONE ((uint8_t)0x00)
#define LOW_TONE    ((uint8_t)0x01)
#define MID_TONE    ((uint8_t)0x02)
#define HIGH_TONE   ((uint8_t)0x03)

static bool tone_enabled = true;
/* The tone the keypad replays. The legacy kept exactly this latch - SetTone()
 * stored tone_type and BuzzerTrigerPrecess() played it on a key press - and
 * defaulted it to LOW_TONE. */
static uint8_t tone_type = LOW_TONE;

void hal_control_tone(uint8_t status, bool play)
{
    if(!play) {
        hal_gpio_set(VOICE_S1_GPIO, GPIO_LOW);
        hal_gpio_set(VOICE_S2_GPIO, GPIO_LOW);
        hal_gpio_set(VOICE_S3_GPIO, GPIO_LOW);
        return;
    }
    
    hal_gpio_set(VOICE_NRST_GPIO, GPIO_LOW);
    hal_gpio_set(VOICE_S4_GPIO, GPIO_LOW);

    hal_gpio_set(VOICE_S1_GPIO, (status == LOW_TONE)  ? GPIO_HIGH : GPIO_LOW);
    hal_gpio_set(VOICE_S2_GPIO, (status == MID_TONE)  ? GPIO_HIGH : GPIO_LOW);
    hal_gpio_set(VOICE_S3_GPIO, (status == HIGH_TONE) ? GPIO_HIGH : GPIO_LOW);

    hal_set_tone_start_timer();
}

uint16_t hal_set_tone_status(uint8_t status)
{
    if(status == TOGGLE_TONE) {
        tone_enabled = !tone_enabled;
        return CMD_RESULT_CODE_SUCCESS;
    }

    if(status < LOW_TONE || status > HIGH_TONE) {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }

    /* A tone request while muted is a valid request that simply produces no
     * sound, not a bad parameter, so it still reports success. Note this makes
     * "played" and "muted" indistinguishable over the wire - the log line
     * below is the only way to tell them apart, which matters because
     * tone_enabled resets to true on every MCU reset while the host's idea of
     * the mute state does not. */
    tone_type = status;
    if(tone_enabled) {
        hal_control_tone(status, true);
    }
    return CMD_RESULT_CODE_SUCCESS;
}

/* Key press feedback. Honours the mute flag, which hal_control_tone() itself
 * does not - the legacy BuzzerTrigerPrecess() checked tone_enable before doing
 * anything, and the keypad is the path that check was written for. */
void hal_play_key_tone(void)
{
    if(tone_enabled) {
        hal_control_tone(tone_type, true);
    }
}

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

/* The four single-pin rails: csafe, RFID 5V, tuner 12V and heart rate 5V. All
 * are active high, and the legacy runtime switch for them is a single GPIO
 * write - the generic branch at the end of PwrPinCtrl(), reached once
 * SetPwrEn() has stored the requested enable bit. The 35-entry PwrFlowCtrl[]
 * boot sequence is a separate concern and is not needed here;
 * hal_gpio_default_state_set() already brings all four up.
 *
 * Any non-zero status enables, matching legacy SetPwrEn(), which collapsed
 * every non-zero byte to 1 rather than rejecting it. Nothing here can fail, so
 * the result is always SUCCESS. */
static uint16_t set_single_pin_power(output_gpio_list_num gpio, uint8_t status)
{
    hal_gpio_set(gpio, (status == POWER_DISABLE) ? GPIO_LOW : GPIO_HIGH);
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_csafe_power(uint8_t status)
{
    return set_single_pin_power(CSAFE_PWR_EN_GPIO, status);
}

uint16_t hal_set_rfid_power(uint8_t status)
{
    return set_single_pin_power(RFID_5V_EN_GPIO, status);
}

uint16_t hal_set_tv_tuner_12v_power(uint8_t status)
{
    return set_single_pin_power(TV_12V_EN_GPIO, status);
}

uint16_t hal_set_hr_rate_power(uint8_t status)
{
    return set_single_pin_power(HR_5V_EN_GPIO, status);
}

/* Qi charging. Two P-FET load switches (Q78/Q79 and Q80/Q81, NCE40P40K back to
 * back) both feed the single QI_POWER net: QI_12W_EN gates +12V_QI at 15W,
 * QI_5W_EN gates +5V_SW at 5W, where +5V_SW is the U94 (MP2329GG) buck off
 * +12V_SW. Both enables are active high.
 *
 * Because both switches drive the same output net, the two must never be on at
 * once - that would tie +12V_QI to the buck's output. Every transition is
 * therefore break-before-make: drop the other rail, wait, then raise the wanted
 * one. The 100ms is the legacy's, and on this topology it is load bearing
 * rather than cosmetic. It blocks the main loop, the same trade
 * hal_set_ext_12v_power() already makes.
 *
 * The board has no 9V rail, so the App's "Default 9V" / "9V" / "12V" labels are
 * wrong. The encoding kept here is the legacy SetQIPower() one - 0x00 selects
 * 5V, 0x01 selects 12V - because the SoC, not the test app, is the real client.
 *
 * Voltage and on/off stay separate commands as in the legacy: 0x0009 latches
 * which rail, and power target 0x09 switches QI_POWER on or off. Choosing a
 * voltage while Qi is off only latches it - legacy SetQIPower() left both pins
 * low in that case - and it takes effect at the next enable. Note ERP_EN also
 * shuts the U94 buck down through Q77, so the 5W rail collapses in ERP
 * regardless of QI_5W_EN. */
#define QI_VOLTAGE_5V       ((uint8_t)0x00)
#define QI_VOLTAGE_12V      ((uint8_t)0x01)

static bool qi_enabled = true;
static uint8_t qi_voltage = QI_VOLTAGE_5V;

static void qi_power_apply(void)
{
    if(!qi_enabled) {
        hal_gpio_set(QI_12W_EN_GPIO, GPIO_LOW);
        hal_gpio_set(QI_5W_EN_GPIO, GPIO_LOW);
        return;
    }

    if(qi_voltage == QI_VOLTAGE_12V) {
        hal_gpio_set(QI_5W_EN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
        hal_gpio_set(QI_12W_EN_GPIO, GPIO_HIGH);
    }
    else {
        hal_gpio_set(QI_12W_EN_GPIO, GPIO_LOW);
        hal_delay_1ms(100);
        hal_gpio_set(QI_5W_EN_GPIO, GPIO_HIGH);
    }
}

uint16_t hal_set_qi_power(uint8_t status)
{
    qi_enabled = (status != POWER_DISABLE);
    qi_power_apply();
    return CMD_RESULT_CODE_SUCCESS;
}

uint16_t hal_set_qi_status(uint8_t status)
{
    if(status != QI_VOLTAGE_5V && status != QI_VOLTAGE_12V) {
        return CMD_RESULT_CODE_INVALID_PARAMETERS;
    }

    qi_voltage = status;
    if(qi_enabled) {
        qi_power_apply();
    }
    return CMD_RESULT_CODE_SUCCESS;
}

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