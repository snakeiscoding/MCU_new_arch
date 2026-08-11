#ifndef MID_LOG_H__
#define MID_LOG_H__

#include <stdint.h>

#define LOG_ERROR   0
#define LOG_WARN    1
#define LOG_INFO    2
#define LOG_VERBOSE 3

#define mid_log_print(level, fmt, ...) mid_log_process(level, fmt, ##__VA_ARGS__)

void mid_log_process(uint8_t level, const char *fmt, ...);

#endif //MID_LOG_H__