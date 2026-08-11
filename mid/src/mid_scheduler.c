#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "hal_timer_handler.h"
#include "mid_scheduler.h"

static event_header_t * m_queue_event_headers;
static uint8_t *m_queue_event_data;
static volatile uint8_t m_queue_start_index;
static volatile uint8_t m_queue_end_index;
static uint16_t m_queue_event_size;
static uint16_t m_queue_size;

static bool is_word_aligned(void const *p)
{
    if(((uintptr_t)p & 0x03) == 0) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t mid_sched_init(uint16_t event_size, uint16_t queue_size, void *p_event_buffer)
{
    uint16_t data_start_index = (queue_size + 1) * sizeof(event_header_t);

    if(!is_word_aligned(p_event_buffer)) {
        return SCH_ERROR_INVALID_PARAM;
    }

    m_queue_event_headers = p_event_buffer;
    m_queue_event_data = &((uint8_t *)p_event_buffer)[data_start_index];
    m_queue_end_index = 0;
    m_queue_start_index = 0;
    m_queue_event_size = event_size;
    m_queue_size = queue_size;

    return SCH_SUCCESS;
}

static uint8_t mid_sched_queue_next_index(uint8_t index)
{
    return (index < m_queue_size) ? (index + 1) : 0;
}

static uint8_t mid_sched_queue_full(void)
{
    uint8_t tmp = m_queue_start_index;
    return mid_sched_queue_next_index(m_queue_end_index) == tmp;
}

static uint8_t mid_sched_queue_empty(void)
{
    uint8_t tmp = m_queue_start_index;
    return m_queue_end_index == tmp;
}

uint32_t mid_sched_event_put(void *p_event_data, uint16_t event_data_size, mid_sched_event_handler_t handler)
{
    uint32_t err_code;

    if(event_data_size <= m_queue_event_size) {
        uint16_t event_index = 0xFFFF;
        // CRITICAL_REGION_ENTER();
        if(!mid_sched_queue_full()) {
            event_index = m_queue_end_index;
            m_queue_end_index = mid_sched_queue_next_index(m_queue_end_index);
#ifdef APP_SCHEDULER_WITH_PROFILER
            // This function call must be protected with critical
            // region because it modifies 'm_max_queue_utilization'.
            queue_utilization_check();
#endif
        }
		// CRITICAL_REGION_EXIT();
        if(event_index != 0xFFFF) {
            // NOTE: This can be done outside the critical region
            // since the event consumer will
            // always be called from the main loop, and will
            // thus never interrupt this code.
            m_queue_event_headers[event_index].handler = handler;
            if((p_event_data != NULL) && (event_data_size > 0)) {
                memcpy(&m_queue_event_data[event_index * m_queue_event_size], p_event_data, event_data_size);
                m_queue_event_headers[event_index].event_data_size = event_data_size;
            }
            else {
                m_queue_event_headers[event_index].event_data_size = 0;
            }
            err_code = SCH_SUCCESS;
        }
        else {
            err_code = SCH_ERROR_NO_MEM;
        }
    }
    else {
        err_code = SCH_ERROR_INVALID_LENGTH;
    }
    return err_code;
}

static uint32_t mid_sched_event_get(void **pp_event_data, uint16_t *p_event_data_size, mid_sched_event_handler_t *p_event_handler)
{
    uint32_t err_code = SCH_ERROR_NOT_FOUND;

    if(!mid_sched_queue_empty()) {
        uint16_t event_index;

        // NOTE: There is no need for a critical region here, as this
        // function will only be called
        // from app_sched_execute() from inside the main loop, so
        // it will never interrupt app_sched_event_put(). Also,
        // updating of (i.e. writing to) the start index will be
        // an atomic operation.
        event_index = m_queue_start_index;
        m_queue_start_index = mid_sched_queue_next_index(m_queue_start_index);

        *pp_event_data = &m_queue_event_data[event_index * m_queue_event_size];
		*p_event_data_size = m_queue_event_headers[event_index].event_data_size;
        *p_event_handler = m_queue_event_headers[event_index].handler;

        err_code = SCH_SUCCESS;
    }

    return err_code;
}

void mid_sched_execute(void)
{
	void *p_event_data;
	uint16_t event_data_size;
	mid_sched_event_handler_t event_handler;

	// Get next event (if any), and execute handler
	while((mid_sched_event_get(&p_event_data, &event_data_size, &event_handler) == SCH_SUCCESS)) {
		event_handler(p_event_data, event_data_size);
	}
}

__attribute__((weak)) void hal_1ms_tick_handler(void *p_event_data, uint16_t event_size) {}
void mid_1ms_tick_handler(void)
{
    mid_sched_event_put((void *)NULL, 0, hal_1ms_tick_handler);
}