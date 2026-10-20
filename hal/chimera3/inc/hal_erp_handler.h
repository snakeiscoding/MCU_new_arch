#ifndef HAL_ERP_HANDLER_H__
#define HAL_ERP_HANDLER_H__

#include <stdint.h>

/* ERP (EuP) low power flow for chimera3.
 *
 * The mid layer state machine (mid_erp_flow.c) drives the sequence; this file
 * supplies the chimera3 hardware actions it hooks into:
 *
 *   SOC_POWER_OFF_STATUS, 3s in   -> hal_set_erp_soc_power_off()
 *                                    drop the SoC (ERP_EN high, ONOFF low)
 *   SOC_POWER_OFF_STATUS, 4s in   -> hal_set_erp_soc_power_off_success_handler()
 *                                    -> hal_set_erp_power_off()
 *                                    every rail off, then MCU STOP mode
 *
 * Wake is a keypad press: before STOP the drive lines are parked low and the
 * two sense lines are rearmed as falling edge EXTI, so any key connects a low
 * drive line to a sense line and the edge wakes the core. The legacy also woke
 * on an IR receiver (PC11); that input does not exist in this architecture.
 * Before STOP, IR_5V_EN is switched on so the IR receiver stays powered, and
 * the power LED (LED_ONOFF) is lit for the whole of ERP, as in the legacy.
 *
 * Two wake sources:
 *   keypad - the drive lines are parked low and the two sense lines rearmed as
 *            falling edge EXTI, so any key connects a low drive line to a
 *            sense line and the edge wakes the core.
 *   IR     - PC11 (REMODE_IR_IN), legacy rules: wake if the line is already
 *            low when STOP is entered, or once it has stayed low for 2s after
 *            an edge; anything shorter goes straight back to STOP.
 *
 * Known limit, same as the legacy: the IWDG cannot be stopped, so if the SoC
 * enabled the MCU watchdog (0x0007) before entering ERP, it expires ~8.2s into
 * STOP and resets the MCU. Test ERP with the MCU watchdog off. */
void hal_set_erp_power_off(void);

/* Called from HAL_GPIO_EXTI_Callback() for the keypad sense lines while the
 * MCU is in (or arming for) STOP mode. */
void hal_erp_wakeup_flag_set(void);

/* Called for every PC11 edge (via hal_ir_rx_edge()). Only looked at inside the
 * STOP loop, which clears it before sleeping. */
void hal_erp_ir_wakeup_flag_set(void);

#endif //HAL_ERP_HANDLER_H__