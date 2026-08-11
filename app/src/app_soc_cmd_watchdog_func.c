#include <stddef.h>
#include <stdio.h>
#include "app_soc_cmd_watchdog_func.h"
#include "app_soc_cmd.h"
#include "mid_log.h"

extern volatile uint64_t time_1ms;

bool soc_watchdog_en = false;
uint64_t soc_watchdog_start_time = 0;
uint32_t soc_watchdog_timeout = 0;

bool app_soc_cmd_watchdog_check(void)
{
    if(soc_watchdog_en) {
        if(time_1ms - soc_watchdog_start_time > soc_watchdog_timeout) {
            mid_log_print(LOG_VERBOSE, "soc watchdog timeout\r\n");
            return true;
        }
    } 
    return false;
}

bool app_soc_cmd_watchdog_get(void)
{
    return soc_watchdog_en;
}

void app_soc_cmd_watchdog_set(uint8_t status)
{
    if(status) {
        soc_watchdog_en = true;
    }
    else {
        soc_watchdog_en = false;
    }
}

void app_soc_cmd_watchdog_func(uint8_t status, uint32_t timeout_ms, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len != 5) {
        mid_soc_cmd_send(CMD_WATCHDOG, CMD_RESULT_CODE_INVALID_PARAMETERS, NULL, 0);
        return;
    }
    if(status) {
        soc_watchdog_start_time = time_1ms;
        soc_watchdog_timeout = timeout_ms;
    }
    app_soc_cmd_watchdog_set(status);
    mid_soc_cmd_send(CMD_WATCHDOG, CMD_RESULT_CODE_SUCCESS, NULL, 0);
}
void app_soc_cmd_watchdog_refresh_func(void)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    soc_watchdog_start_time = time_1ms;
    mid_soc_cmd_send(CMD_WATCHDOG_REFRESH, CMD_RESULT_CODE_SUCCESS, NULL, 0);
}
