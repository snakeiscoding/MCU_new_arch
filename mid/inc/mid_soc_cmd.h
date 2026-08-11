#ifndef MID_SOC_CMD_H__
#define MID_SOC_CMD_H__

#include <stdint.h>

typedef struct SOC_CMD {
	uint16_t cmd;
    uint16_t cmd_data_len;
    uint8_t cmd_data_buffer[1024];
} SOC_CMD;

#define CMD_RESULT_CODE_SUCCESS                 ((uint16_t)0x0000)
#define CMD_RESULT_CODE_UNKNOWN_COMMAND         ((uint16_t)0x0001)
#define CMD_RESULT_CODE_INVALID_PAYLOAD_LENGTH  ((uint16_t)0x0002)
#define CMD_RESULT_CODE_INVALID_PARAMETERS      ((uint16_t)0x0003)
#define CMD_RESULT_CODE_INVALID_ADDRESS         ((uint16_t)0x0004)
#define CMD_RESULT_CODE_INVALID_PASSWORD        ((uint16_t)0x0005)
#define CMD_RESULT_CODE_FAILURE                 ((uint16_t)0x0006)

void mid_soc_cmd_receive(void *p_event_data, uint16_t event_size);
void mid_soc_cmd_send(uint16_t cmd_id, uint16_t cmd_result, uint8_t *cmd_data, uint16_t cmd_data_len);

#endif //MID_SOC_CMD_H__