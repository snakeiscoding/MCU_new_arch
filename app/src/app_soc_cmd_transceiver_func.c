#include <stddef.h>
#include "app_soc_cmd_transceiver_func.h"
#include "app_soc_cmd.h"
#include "mid_csafe_cab.h"
#include "mid_bump_sensor.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

__attribute__((weak)) void mid_send_soc_cmd_to_bump_sensor(uint8_t *data, uint16_t len) {}
void app_soc_cmd_transceiver_bump_sensor(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    mid_send_soc_cmd_to_bump_sensor(data, len);
}

/* Legacy SendIRCmdReplyFun() acked every IR command; this does the same with
 * whatever the HAL returns. Targets without IR keep the stub's UNKNOWN_COMMAND. */
__attribute__((weak)) uint16_t hal_send_ir_command(uint8_t *data, uint16_t len) {return CMD_RESULT_CODE_UNKNOWN_COMMAND;}
void app_soc_cmd_transceiver_ir(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    uint16_t result = hal_send_ir_command(data, len);
    mid_soc_cmd_send(CMD_TRAMSCEIVER_SEND_IR_COMMAND, result, NULL, 0);
}

__attribute__((weak)) void mid_send_soc_cmd_to_csafe_cab(uint8_t *data, uint16_t len) {}
void app_soc_cmd_transceiver_csafe_cab(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    mid_send_soc_cmd_to_csafe_cab(data, len);
}

