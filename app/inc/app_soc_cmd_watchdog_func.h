#ifndef APP_SOC_CMD_WATCHDOG_FUNC_H__
#define APP_SOC_CMD_WATCHDOG_FUNC_H__

#include <stdbool.h>
#include <stdint.h>

bool app_soc_cmd_watchdog_check(void);
bool app_soc_cmd_watchdog_get(void);
void app_soc_cmd_watchdog_func(uint8_t status, uint32_t timeout_ms, uint16_t len);
void app_soc_cmd_watchdog_refresh_func(void);

#endif //APP_SOC_CMD_WATCHDOG_FUNC_H__