#ifndef APP_SOC_CMD_TRAMSCEIVER_FUNC_H__
#define APP_SOC_CMD_TRAMSCEIVER_FUNC_H__

#include <stdint.h>

void app_soc_cmd_transceiver_bump_sensor(uint8_t *data, uint16_t len);
void app_soc_cmd_transceiver_ir(uint8_t *data, uint16_t len);
void app_soc_cmd_transceiver_csafe_cab(uint8_t *data, uint16_t len);

#endif //APP_SOC_CMD_TRAMSCEIVER_FUNC_H__