#ifndef HAL_SOC_RX_H__
#define HAL_SOC_RX_H__

#include <stdint.h>

/* SoC command receive path for chimera3, following the legacy v0.18
 * SocUart.c.
 *
 * USART1 bytes are framed in the RX interrupt: a frame starts at 55 AA, its
 * length comes from the header, and each complete frame goes into a 16 slot
 * queue. The 1ms tick drains that queue, at most 4 frames per tick, and hands
 * each frame to mid_soc_cmd_receive(), which checks the CRC and dispatches.
 * Noise before 55 AA is skipped, several frames in one burst are all kept,
 * and an oversized length or a full queue drops only the frame concerned. */

/* Drop any partial frame and every queued one. Run from usart1_init(), so it
 * happens at boot and again on ERP wake, as v0.18 did in SoCUartGpioStart(). */
void hal_soc_rx_reset(void);

/* One received byte, from USART1_IRQHandler(). */
void hal_soc_rx_byte(uint8_t byte);

/* Dispatch queued frames; called from the 1ms tick handler. */
void hal_soc_rx_process(void);

#endif //HAL_SOC_RX_H__
