#ifndef MID_CAN_CMD_H__
#define MID_CAN_CMD_H__

#include <stdint.h>

void mid_can_cmd_receive(void *p_event_data, uint16_t event_size);

#endif //MID_CAN_CMD_H__