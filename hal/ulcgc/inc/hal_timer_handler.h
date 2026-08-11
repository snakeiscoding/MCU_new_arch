#ifndef HAL_TIMER_HANDLER_H__
#define HAL_TIMER_HANDLER_H__
#include <stdint.h>

typedef enum {
    CURR_MON_IDLE = 0,
    CURR_MON_STAGE1,
    CURR_MON_STAGE2,
} CURR_MON_STATE;

void hal_set_buzzer_start_timer(uint32_t time);
void hal_set_current_monitor(bool enable);
void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size);

#endif //HAL_TIMER_HANDLER_H__