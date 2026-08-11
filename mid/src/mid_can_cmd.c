#include "hal_can.h"
#include "mid_can_cmd.h"

__attribute__((weak)) void hal_can_cmd_send(void *p_event_data, uint16_t event_size) {}
void mid_can_cmd_receive(void *p_event_data, uint16_t event_size)
{
    hal_can_cmd_send(p_event_data, event_size);
}