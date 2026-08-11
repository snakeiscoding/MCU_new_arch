#include "hal_timer_handler.h"
#include "mid_erp_flow.h"

void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size)
{
    mid_erp_status_handler();
    /*mid_tca8418_keypad_read_handler();*/
}