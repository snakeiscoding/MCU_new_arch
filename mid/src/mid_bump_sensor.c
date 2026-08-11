#include "app_soc_cmd.h"
#include "mid_bump_sensor.h"
#include "mid_soc_cmd.h"

__attribute__((weak)) void hal_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len) {}
void mid_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len)
{
    hal_send_soc_cmd_to_bump_sensor(data, len);
}

void mid_bump_sensor_response_to_soc(void *p_event_data, uint16_t event_size)
{
    uint8_t *data = (uint8_t *)p_event_data;
	uint16_t data_len = event_size;
    mid_soc_cmd_send(CMD_TRAMSCEIVER_SEND_BUMP_SENSOR_COMMAND, CMD_RESULT_CODE_SUCCESS, data, data_len);
}