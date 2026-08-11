#include <stdarg.h>
#include <stdio.h>
#include "mid_log.h"

extern volatile uint64_t time_1ms;
int currentLogLevel = LOG_VERBOSE;

static const char *mid_log_level_to_string(uint8_t level)
{
    switch(level) {
    case LOG_ERROR:
        return "ERROR";
    case LOG_WARN:
        return "WARN";
    case LOG_INFO:
        return "INFO";
    case LOG_VERBOSE:
        return "VERBOSE";
    default:                
        return "UNKNOWN";
    }
}

void mid_log_process(uint8_t level, const char *fmt, ...)
{
    if(level > currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    uint32_t high = (uint32_t)(time_1ms / 1000000000); 
    uint32_t low  = (uint32_t)(time_1ms % 1000000000);
    if(high > 0) {
        fprintf(stderr, "[%u%09u][%s] ", (unsigned int)high, (unsigned int)low, mid_log_level_to_string(level));
    } 
    else {
        fprintf(stderr, "[%u][%s] ", (unsigned int)low, mid_log_level_to_string(level));
    }
    vfprintf(stderr, fmt, args);
    va_end(args);
}