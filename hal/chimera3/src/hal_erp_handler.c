#include <stdbool.h>
#include "hal_erp_handler.h"
#include "hal_gpio.h"
#include "hal_soc_cmd_interface.h"
#include "hal_system.h"
#include "mid_log.h"

static volatile bool erp_wakeup_flag = false;

void hal_erp_wakeup_flag_set(void)
{
    erp_wakeup_flag = true;
}

/* The rail shutdown the legacy PowerERPProcess(Into_ERP_Mode) performed,
 * restricted to the rails this architecture actually defines. The legacy also
 * turned IR_5V_EN on here (the IR receiver stayed powered as a wake source)
 * and touched HDMI / audio / USB VBUS / auth rails that have no pin definition
 * here yet; whoever adds those pins should add them to this list. */
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

/* The 4s mark of SOC_POWER_OFF_STATUS: the SoC rail is already down (the 3s
 * hook in hal_soc_cmd_interface.c dropped it), so finish the job - every other
 * rail off, keypad armed as the wake source, then STOP mode. Runs in scheduler
 * context and blocks there for the whole low power period, exactly as the
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
    hal_erp_arm_keypad_wakeup();

    erp_wakeup_flag = false;
    HAL_SuspendTick();
    while(!erp_wakeup_flag) {
        /* Any enabled interrupt exits WFI; the flag filters the wake sources
         * down to the keypad EXTI lines. */
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    }
    HAL_ResumeTick();

    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    hal_system_init();

    mid_log_print(LOG_INFO, "ERP: woke from STOP mode\r\n");
}