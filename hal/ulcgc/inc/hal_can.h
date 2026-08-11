#ifndef HAL_CAN_H__
#define HAL_CAN_H__

#include <stdint.h>

void hal_can_init(void);
void hal_can_deinit(void);
void hal_can_cmd_send(void *p_event_data, uint16_t event_size);

#endif //HAL_CAN_H__