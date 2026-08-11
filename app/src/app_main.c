#include <stdio.h>
#include "app_soc_cmd_info_func.h"
#include "app_system.h"
#include "mid_log.h"
#include "mid_scheduler.h"
#include "hal_adc.h"

volatile uint64_t time_1ms = 0;
int main(void)
{
    MID_SCHED_INIT(1024, 50);
    app_system_init();

    const char* project = app_get_project_name();
    uint16_t version = app_get_fw_version();
    mid_log_print(LOG_INFO, "%s Application\r\n", project);
    mid_log_print(LOG_INFO, "%s Hardware\r\n", 
        (hal_get_hw_id() == hw_k1) ? "K1" : 
        (hal_get_hw_id() == hw_k2) ? "K2" : 
        (hal_get_hw_id() == hw_k3) ? "K3" : "Unknown");
    mid_log_print(LOG_INFO, "V%d.%d build %s %s\r\n", (version & 0xFF00) >> 8, version & 0x00FF, __DATE__, __TIME__);
    while(1) {
        mid_sched_execute();
    }
}
