#ifndef MID_BUMP_SENSOR_H__
#define MID_BUMP_SENSOR_H__

#include <stdint.h>

void mid_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len);
void mid_bump_sensor_response_to_soc(void *p_event_data, uint16_t event_size);

#endif //MID_BUMP_SENSOR_H__