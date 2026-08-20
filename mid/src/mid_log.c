#include <stdarg.h>
#include <stdio.h>
#include "mid_log.h"

extern volatile uint64_t time_1ms;
/* Starts at LOG_VERBOSE so a target that never receives the debug command
 * behaves exactly as before. app_soc_cmd_control_debug_func() overrides it. */
static int currentLogLevel = LOG_VERBOSE;

void mid_log_set_level(int level)
{
    if(level < LOG_SILENT) {
        level = LOG_SILENT;
    }
    else if(level > LOG_VERBOSE) {
        level = LOG_VERBOSE;
    }
    currentLogLevel = level;
}

int mid_log_get_level(void)
{
    return currentLogLevel;
}

bool mid_log_level_enabled(uint8_t level)
{
    return (int)level <= currentLogLevel;
}

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