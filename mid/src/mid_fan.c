#include <stdarg.h>
#include <stdio.h>
#include "mid_fan.h"
#include "mid_log.h"

extern volatile uint64_t time_1ms;

uint32_t fan_rpm_record[100] = {0};
uint64_t the_calcu_last_time = 0;
uint8_t fan_rpm_record_index = 0;
uint32_t mid_get_fan_rpm(void)
{
    uint32_t fan_rpm = 0;
    for(int i = 0; i < 100; i++) {
        fan_rpm += fan_rpm_record[i];
    }
    mid_log_print(LOG_INFO, "fan rpm : %ld\r\n", fan_rpm / 100);
	return fan_rpm / 100;
}

void mid_calcu_fan_rpm(void)
{
    fan_rpm_record[fan_rpm_record_index] = (1000 * 60) / ((time_1ms - the_calcu_last_time) * 4);
    the_calcu_last_time = time_1ms;
    fan_rpm_record_index++;
    if(fan_rpm_record_index > 99) {
        fan_rpm_record_index = 0;
    }
}