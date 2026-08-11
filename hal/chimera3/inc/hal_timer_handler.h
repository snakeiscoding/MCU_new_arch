#ifndef HAL_TIMER_HANDLER_H__
#define HAL_TIMER_HANDLER_H__

#include <stdint.h>

void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size);

#endif //HAL_TIMER_HANDLER_H__