#ifndef HAL_IR_H__
#define HAL_IR_H__

#include <stdint.h>

/* IR for chimera3, ported from the legacy IRCtrl.c / DirButton.c.
 *
 * Transmit: STB_IR (PA1) carries the IR carrier from TIM5 CH2, and TIM7 at
 * 1us per tick times each mark and space. The SoC sends the timing table with
 * 0x0705 (the legacy number for it was 0x0601), and the whole frame is clocked
 * out from the TIM7 interrupt.
 *
 * Receive: REMODE_IR_IN (PC11) is EXTI on both edges. While the SoC is up the
 * edges are only counted and logged, the same as the legacy printf did. In ERP
 * the same line is a wake source - see hal_erp_handler.c. The receiver gets
 * its power from IR_5V_EN (PA11), which is power target 0x11 and is also
 * switched on at ERP entry. */
void hal_ir_init(void);

/* Payload layout and quirks are documented next to the implementation. */
uint16_t hal_send_ir_command(uint8_t *data, uint16_t len);

/* For ERP entry: abort any frame still being sent and release STB_IR to its
 * reset state (input), as the legacy did by deinitialising TIM5. hal_ir_init()
 * on wake takes it back. */
void hal_ir_deinit(void);

/* Called from HAL_GPIO_EXTI_Callback() for every PC11 edge. */
void hal_ir_rx_edge(void);

void hal_ir_handler_1ms(void);

#endif //HAL_IR_H__
