#ifndef HAL_TIMER_HANDLER_H__
#define HAL_TIMER_HANDLER_H__

#include <stdint.h>

/* Arms the 20ms buzzer release; called by hal_control_tone(). */
void hal_set_tone_start_timer(void);
void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size);

#endif //HAL_TIMER_HANDLER_H__