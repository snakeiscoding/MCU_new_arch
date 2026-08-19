#ifndef HAL_KEY_HANDLER_H__
#define HAL_KEY_HANDLER_H__

#include <stdint.h>

/* Matrix keypad and safety key, both polled from the 1ms tick.
 *
 * hal_key_handler_1ms() advances one step of the keypad scan and takes one
 * sample of the safety key. It must be called from hal_1ms_tick_handler(). */
void hal_key_handler_1ms(void);

/* Raw safety key pin level: 1 = clear, 0 = triggered. Overrides the weak hook
 * declared in app_soc_cmd_event_func.c, which maps it to the wire values. */
uint8_t hal_get_estop_status(void);

#endif //HAL_KEY_HANDLER_H__