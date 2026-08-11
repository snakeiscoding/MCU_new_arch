#ifndef MID_SCHEDULER_H__
#define MID_SCHEDULER_H__

#include <stdint.h>

typedef void (*mid_sched_event_handler_t)(void *p_event_data, uint16_t event_size);
typedef struct {
    mid_sched_event_handler_t handler;
    uint16_t                  event_data_size;
} event_header_t;

#define CEIL_DIV(A, B) (((A) + (B) - 1) / (B))

#define SCH_ERROR_BASE_NUM (0x0)
#define SCH_SUCCESS (SCH_ERROR_BASE_NUM + 0)
#define SCH_ERROR_NO_MEM (SCH_ERROR_BASE_NUM + 1)
#define SCH_ERROR_NOT_FOUND (SCH_ERROR_BASE_NUM + 2)
#define SCH_ERROR_INVALID_PARAM (SCH_ERROR_BASE_NUM + 3)
#define SCH_ERROR_INVALID_LENGTH (SCH_ERROR_BASE_NUM + 4)

#define MID_SCHED_EVENT_HEADER_SIZE 8
#define MID_SCHED_BUF_SIZE(EVENT_SIZE, QUEUE_SIZE) (((EVENT_SIZE) + MID_SCHED_EVENT_HEADER_SIZE) * ((QUEUE_SIZE) + 1))
#define MID_SCHED_INIT(EVENT_SIZE, QUEUE_SIZE)  \
    do {    \
		static uint32_t MID_SCHED_BUF[CEIL_DIV(MID_SCHED_BUF_SIZE((EVENT_SIZE), (QUEUE_SIZE)), sizeof(uint32_t))];  \
        mid_sched_init((EVENT_SIZE), (QUEUE_SIZE), MID_SCHED_BUF);  \
    } while (0)

uint32_t mid_sched_init(uint16_t max_event_size, uint16_t queue_size, void *p_evt_buffer);
uint32_t mid_sched_event_put(void *p_event_data, uint16_t event_size, mid_sched_event_handler_t handler);
void mid_sched_execute(void);
void mid_1ms_tick_handler(void);

#endif //MID_SCHEDULER_H__