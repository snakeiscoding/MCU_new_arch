#include <stdbool.h>
#include "hal_erp_handler.h"
#include "hal_gpio.h"
#include "hal_ir.h"
#include "hal_soc_cmd_interface.h"
#include "hal_sysclk.h"
#include "hal_systick.h"
#include "hal_system.h"
#include "mid_log.h"

extern volatile uint64_t time_1ms;

static volatile bool erp_wakeup_flag = false;
static volatile bool erp_ir_wakeup_flag = false;

void hal_erp_wakeup_flag_set(void)
{
    erp_wakeup_flag = true;
}

void hal_erp_ir_wakeup_flag_set(void)
{
    erp_ir_wakeup_flag = true;
}

/* The rail shutdown the legacy PowerERPProcess(Into_ERP_Mode) performed,
 * restricted to the rails this architecture actually defines. The legacy also
 * touched HDMI / audio / USB VBUS / auth rails that have no pin definition
 * here yet; whoever adds those pins should add them to this list. IR_5V_EN is
 * the one rail ERP turns on rather than off - see hal_erp_ir_power_on(). */
static void hal_erp_rails_off(void)
{
    hal_gpio_set(EXT_EN_GPIO, GPIO_LOW);
    hal_gpio_set(EXT12V_EN_GPIO, GPIO_LOW);
    hal_gpio_set(MCB_EN_GPIO, GPIO_LOW);
    hal_gpio_set(TV_12V_EN_GPIO, GPIO_LOW);
    hal_gpio_set(HR_5V_EN_GPIO, GPIO_LOW);
    hal_gpio_set(CSAFE_PWR_EN_GPIO, GPIO_LOW);
    hal_gpio_set(RFID_5V_EN_GPIO, GPIO_LOW);
    hal_gpio_set(QI_5W_EN_GPIO, GPIO_LOW);
    hal_gpio_set(QI_12W_EN_GPIO, GPIO_LOW);
    hal_gpio_set(LCB_RS485M_EN_GPIO, GPIO_LOW);
    hal_gpio_set(LCB_RS485C_EN_GPIO, GPIO_LOW);
}

/* Park the keypad for wake: every drive line low, both sense lines rearmed as
 * falling edge interrupts. A pressed key then shorts a low drive line onto a
 * sense line and the falling edge is the wake event. This deliberately
 * bypasses hal_input_gpio_init(), whose table configures the sense lines as
 * plain polled inputs; hal_system_init() puts them back after wake. */
static void hal_erp_arm_keypad_wakeup(void)
{
    hal_gpio_set(KEYH_D1_GPIO, GPIO_LOW);
    hal_gpio_set(KEYH_D2_GPIO, GPIO_LOW);
    hal_gpio_set(KEYH_D3_GPIO, GPIO_LOW);
    hal_gpio_set(KEYH_D4_GPIO, GPIO_LOW);
    hal_gpio_set(KEYH_D5_GPIO, GPIO_LOW);
    hal_gpio_set(KEYH_D6_GPIO, GPIO_LOW);

    GPIO_InitTypeDef GPIO_InitStruct;
    HAL_GPIO_DeInit(GPIOE, KEYV_D1_GPIO_PIN | KEYV_D2_GPIO_PIN);
    GPIO_InitStruct.Pin = KEYV_D1_GPIO_PIN | KEYV_D2_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* Tail of the legacy PowerERPProcess(Into_ERP_Mode): power the IR receiver so
 * it can wake the board, give it the legacy's 150ms to settle, then light the
 * power LED for the rest of ERP. Both go back off on wake, when
 * hal_gpio_default_state_set() runs again. Any IR frame still being sent is
 * dropped first and STB_IR released, as the legacy SleepModeProcess() did by
 * deinitialising TIM5/TIM7. */
static void hal_erp_ir_power_on(void)
{
    hal_ir_deinit();
    hal_gpio_set(IR_5V_EN_GPIO, GPIO_HIGH);
    hal_delay_1ms(150);
    hal_gpio_set(LED_ONOFF_GPIO, GPIO_HIGH);
}

/* The legacy IR wake rule. Its EXTI callbacks stamped the falling edge and,
 * on the rising edge, left ERP only if the line had been low for 2000ms,
 * otherwise put the MCU straight back to sleep; ErpLedProcess() also left ERP
 * once the line had stayed low for 2000ms without a rising edge.
 *
 * Measuring 2s needs a running tick, and STOP exits on the 16MHz HSI with the
 * tick suspended, so the PLL is brought back first. If the line goes high
 * early the tick is suspended again and the caller re-enters STOP; the clock
 * tree can stay as it is, STOP stops the PLL anyway. */
#define ERP_IR_WAKEUP_HOLD_MS   ((uint64_t)2000)

static bool hal_erp_ir_hold_qualified(void)
{
    hal_sysclk_config();
    HAL_ResumeTick();

    uint64_t start = time_1ms;
    while(hal_gpio_get(IR_IN_GPIO) == GPIO_LOW) {
        if((time_1ms - start) >= ERP_IR_WAKEUP_HOLD_MS) {
            return true;
        }
    }

    HAL_SuspendTick();
    return false;
}

/* The 4s mark of SOC_POWER_OFF_STATUS: the SoC rail is already down (the 3s
 * hook in hal_soc_cmd_interface.c dropped it), so finish the job - every other
 * rail off, keypad and IR armed as the wake sources, then STOP mode. Runs in
 * scheduler context and blocks there for the whole low power period, as the
 * legacy blocked in SleepModeProcess() and vortex2 blocks in its
 * hal_set_erp_power_off().
 *
 * On wake everything is rebuilt through hal_system_init(): STOP exits on HSI,
 * so the clock tree, GPIO defaults (which restore every rail and power the SoC
 * back on) and peripherals all need their boot time init again. That also
 * resets the ERP state machine to ENTER_ERP_READY_STATUS, which runs the LCB
 * leave-ERP handshake; with no LCB attached its retries expire and the flow
 * still lands in LEAVE_ERP_READY_STATUS. */
void hal_set_erp_power_off(void)
{
    mid_log_print(LOG_INFO, "ERP: rails off, entering STOP mode\r\n");

    hal_erp_rails_off();
    hal_erp_ir_power_on();
    hal_erp_arm_keypad_wakeup();
    /* PC11 is already EXTI on both edges from boot; re-initialising it here
     * mirrors the legacy, which re-armed it next to the keypad before STOP. */
    hal_input_gpio_init(IR_IN_GPIO);

    const char *wake_source = "keypad";
    erp_wakeup_flag = false;
    erp_ir_wakeup_flag = false;
    HAL_SuspendTick();
    while(!erp_wakeup_flag) {
        /* Legacy SleepModeProcess() only entered STOP with the IR line high;
         * found low, it left ERP on the spot. */
        if(hal_gpio_get(IR_IN_GPIO) == GPIO_LOW) {
            wake_source = "IR (line low at STOP entry)";
            break;
        }

        /* Any enabled interrupt exits WFI; the flags filter the wake sources
         * down to the keypad lines and the IR line. */
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

        if(erp_ir_wakeup_flag) {
            erp_ir_wakeup_flag = false;
            if(hal_erp_ir_hold_qualified()) {
                wake_source = "IR (line held low 2s)";
                break;
            }
        }
    }
    HAL_ResumeTick();

    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    hal_system_init();

    mid_log_print(LOG_INFO, "ERP: woke from STOP mode by %s\r\n", wake_source);
}