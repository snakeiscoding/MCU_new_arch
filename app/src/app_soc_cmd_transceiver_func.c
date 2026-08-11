#include "app_soc_cmd_transceiver_func.h"
#include "app_soc_cmd.h"
#include "mid_csafe_cab.h"
#include "mid_bump_sensor.h"
#include "mid_log.h"

__attribute__((weak)) void mid_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len) {}
void app_soc_cmd_transceiver_bump_sensor(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    mid_send_soc_cmd_to_bump_sensor(data, len);
}

__attribute__((weak)) void mid_send_soc_cmd_to_csafe_cab(uint8_t *data, uint16_t len) {}
void app_soc_cmd_transceiver_csafe_cab(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    mid_send_soc_cmd_to_csafe_cab(data, len);
}

