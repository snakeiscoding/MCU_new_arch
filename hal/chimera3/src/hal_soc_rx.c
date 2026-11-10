#include <string.h>
#include "hal_soc_rx.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"
#include "stm32f4xx.h"

/* Sizes are the legacy v0.18 ones: SOC_CMD_SIZE 0x280 per slot, a queue of
 * SOC_QUEUE_SIZE 16 (one more slot than that, so a full queue still leaves the
 * slot the RX interrupt is filling), and at most 4 frames per processing pass.
 * A frame is 55 AA, cmd (2), result (2), payload length (2), payload, CRC8. */
#define SOC_RX_SLOT_SIZE        ((uint16_t)0x280)
#define SOC_RX_QUEUE_SIZE       ((uint8_t)16)
#define SOC_RX_BUDGET           ((uint8_t)4)
#define SOC_RX_HEADER_LEN       ((uint16_t)8)
#define SOC_RX_FRAME_OVERHEAD   ((uint16_t)9)

typedef struct {
    uint16_t len;
    uint8_t data[SOC_RX_SLOT_SIZE];
} soc_rx_slot_t;

static soc_rx_slot_t soc_rx_queue[SOC_RX_QUEUE_SIZE + 1];
static volatile uint8_t soc_rx_start = 0;   /* advanced by hal_soc_rx_process() */
static volatile uint8_t soc_rx_end = 0;     /* advanced by the RX interrupt */

/* Frame being assembled; touched only by the RX interrupt and by reset. */
static uint16_t soc_rx_count = 0;
static uint16_t soc_rx_payload_len = 0;

/* The legacy printf'd these from the interrupt; here the interrupt only counts
 * and hal_soc_rx_process() logs. */
static volatile uint32_t soc_rx_overflow_cnt = 0;
static uint32_t soc_rx_overflow_logged = 0;
static volatile uint32_t soc_rx_oversize_cnt = 0;
static volatile uint16_t soc_rx_oversize_len = 0;
static uint32_t soc_rx_oversize_logged = 0;

/* Copy of the frame being dispatched. */
static uint8_t soc_rx_frame[SOC_RX_SLOT_SIZE];

static uint8_t soc_rx_next(uint8_t index)
{
    return (index < SOC_RX_QUEUE_SIZE) ? (uint8_t)(index + 1) : 0;
}

void hal_soc_rx_reset(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    soc_rx_start = 0;
    soc_rx_end = 0;
    soc_rx_count = 0;
    soc_rx_payload_len = 0;
    soc_rx_overflow_cnt = 0;
    soc_rx_overflow_logged = 0;
    soc_rx_oversize_cnt = 0;
    soc_rx_oversize_logged = 0;
    if(!primask) {
        __enable_irq();
    }
}

/* The legacy SocUartCmdHandle() state machine. Byte 0 must be 0x55 and byte 1
 * 0xAA; any other byte there restarts the search, and, as in v0.18, a byte
 * that breaks the 0xAA check is not itself re-tried as a 0x55. The frame is
 * kept exactly as received - v0.18 rebuilt the header with the two result
 * bytes forced to 00 before its CRC check, which only differs when the SoC
 * sends a non-zero result. */
void hal_soc_rx_byte(uint8_t byte)
{
    uint8_t *frame = soc_rx_queue[soc_rx_end].data;

    if(soc_rx_count == 0) {
        if(byte == 0x55) {
            frame[soc_rx_count++] = byte;
        }
        return;
    }
    if(soc_rx_count == 1) {
        if(byte == 0xAA) {
            frame[soc_rx_count++] = byte;
        }
        else {
            soc_rx_count = 0;
        }
        return;
    }

    frame[soc_rx_count++] = byte;

    if(soc_rx_count == SOC_RX_HEADER_LEN) {
        soc_rx_payload_len = (uint16_t)frame[6] | ((uint16_t)frame[7] << 8);
        /* Same limit as the legacy: (DtaLen + 11) > SOC_CMD_SIZE. */
        if((uint32_t)soc_rx_payload_len + 11 > SOC_RX_SLOT_SIZE) {
            soc_rx_oversize_len = (uint16_t)(soc_rx_payload_len + 11);
            soc_rx_oversize_cnt++;
            soc_rx_count = 0;
        }
        return;
    }

    if(soc_rx_count < SOC_RX_HEADER_LEN ||
       soc_rx_count < soc_rx_payload_len + SOC_RX_FRAME_OVERHEAD) {
        return;
    }

    if(soc_rx_next(soc_rx_end) != soc_rx_start) {
        soc_rx_queue[soc_rx_end].len = soc_rx_count;
        __DMB();
        soc_rx_end = soc_rx_next(soc_rx_end);
    }
    else {
        soc_rx_overflow_cnt++;
    }
    soc_rx_count = 0;
}

/* The legacy SocUartCmdProcess(): up to 4 frames per pass. Each frame is
 * copied out and its slot released before dispatch, so a long-running command
 * cannot hold the queue, and a reset from inside a handler (ERP wake runs
 * hal_system_init()) leaves nothing half consumed. */
void hal_soc_rx_process(void)
{
    if(soc_rx_oversize_cnt != soc_rx_oversize_logged) {
        soc_rx_oversize_logged = soc_rx_oversize_cnt;
        mid_log_print(LOG_ERROR, "SoC cmd data len oversize : %u\r\n",
                      (unsigned int)soc_rx_oversize_len);
    }
    if(soc_rx_overflow_cnt != soc_rx_overflow_logged) {
        soc_rx_overflow_logged = soc_rx_overflow_cnt;
        mid_log_print(LOG_ERROR, "SoC cmd queue overflow : %lu\r\n",
                      (unsigned long)soc_rx_overflow_logged);
    }

    uint8_t budget = SOC_RX_BUDGET;
    while(soc_rx_start != soc_rx_end && budget--) {
        uint8_t index = soc_rx_start;
        __DMB();
        uint16_t len = soc_rx_queue[index].len;
        memcpy(soc_rx_frame, soc_rx_queue[index].data, len);
        soc_rx_start = soc_rx_next(index);

        mid_soc_cmd_receive(soc_rx_frame, len);
    }
}
