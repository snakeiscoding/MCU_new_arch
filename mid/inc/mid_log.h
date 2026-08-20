#ifndef MID_LOG_H__
#define MID_LOG_H__

#include <stdbool.h>
#include <stdint.h>

#define LOG_ERROR   0
#define LOG_WARN    1
#define LOG_INFO    2
#define LOG_VERBOSE 3
/* Below LOG_ERROR, so it discards every level including errors. Reached only
 * through the SoC's 0x0005 debug command with the enable flag cleared - that
 * is what "debug off" means, as distinct from "errors only", which is
 * LOG_ERROR. */
#define LOG_SILENT  (-1)

#define mid_log_print(level, fmt, ...) mid_log_process(level, fmt, ##__VA_ARGS__)

void mid_log_process(uint8_t level, const char *fmt, ...);

/* Runtime verbosity, driven by the SoC's 0x0005 debug command. The levels that
 * command carries are the LOG_* values above, one for one. Out-of-range values
 * are clamped rather than rejected - the app layer validates first, and this is
 * shared by every target. */
void mid_log_set_level(int level);
int mid_log_get_level(void);

/* For output built piecemeal rather than in a single mid_log_print() - the
 * per-byte command dumps in mid_soc_cmd.c are the only such callers - so they
 * can skip work the level would discard anyway. mid_log_print() filters only
 * what passes through it, not a bare printf() next to it. */
bool mid_log_level_enabled(uint8_t level);

#endif //MID_LOG_H__