#include <stdio.h>
#include <string.h>
#include "hal_lcb_cmd_interface.h"
#include "mid_log.h"
#include "mid_lcb_cmd.h"

static LCB_CMD m_lcb_cmd;

#define LCB_CMD_SEND_BYTE       ((uint8_t)0x00)
#define LCB_CMD_RECEIVE_BYTE    ((uint8_t)0x01)

#define LCB_CMD_PARSER_SUCCESS   0
#define LCB_CMD_PARSER_FAIL      1

#define LCB_CMD_DEBUG

__attribute__((weak)) void hal_lcb_wakeup_init(void) {}
void mid_lcb_wakeup_init(void)
{
    hal_lcb_wakeup_init();
}

static uint8_t lcb_cmd_crc8_table[16] = { 0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97, 0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E };
static uint8_t mid_lcb_cmd_get_crc8(uint8_t *data, uint16_t len)
{
    uint16_t crc_count;
    uint8_t high_nibble, data_temp, new_crc = 0;

    crc_count = len;
    while (crc_count-- != 0) {
        data_temp = *data;
        high_nibble = new_crc / 16;
        new_crc <<= 4;
        new_crc ^= lcb_cmd_crc8_table[high_nibble ^ (data_temp / 16)];
        high_nibble = new_crc / 16;
        new_crc <<= 4;

        new_crc ^= lcb_cmd_crc8_table[high_nibble ^ (data_temp & 0x0F)];
        data++;
    }

	return new_crc;
}

int mid_lcb_cmd_parser(uint8_t *data, uint16_t data_len)
{
    if(data_len < 5) {
        mid_log_print(LOG_ERROR, "LCB command length too short!\r\n");
        return LCB_CMD_PARSER_FAIL;
    }

    if(data[0] != LCB_CMD_RECEIVE_BYTE) {
        mid_log_print(LOG_ERROR, "LCB command receive header error!\r\n");
        return LCB_CMD_PARSER_FAIL;
    }

    m_lcb_cmd.cmd_data_len = (uint16_t)data[3];
    if(m_lcb_cmd.cmd_data_len + 5 != data_len) {
        mid_log_print(LOG_ERROR, "LCB command length error!\r\n");
        return LCB_CMD_PARSER_FAIL;
    }

    uint8_t crc8_data = mid_lcb_cmd_get_crc8(data, data_len - 1);
#ifdef LCB_CMD_DEBUG
    mid_log_print(LOG_INFO, "LCB command crc : 0x%02X\r\n", crc8_data);
#endif

    m_lcb_cmd.cmd = (uint16_t)data[2];
    memcpy((void *)m_lcb_cmd.cmd_data_buffer, (void *)&data[4], m_lcb_cmd.cmd_data_len);

    return LCB_CMD_PARSER_SUCCESS;
}

__attribute__((weak)) void hal_lcb_cmd_handler(const LCB_CMD *p_lcb_cmd) {}
void mid_lcb_cmd_receive(void *p_event_data, uint16_t event_size)
{
	uint8_t *data = (uint8_t *)p_event_data;
	uint16_t data_len = event_size;
#ifdef LCB_CMD_DEBUG
    mid_log_print(LOG_INFO, "LCB cmd rx : ");
    for (int i = 0; i < data_len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\r\n");
#endif

    if(mid_lcb_cmd_parser(data, data_len) == LCB_CMD_PARSER_SUCCESS) {
        hal_lcb_cmd_handler(&m_lcb_cmd);
    }
}

uint16_t mid_lcb_cmd_gen(uint16_t command_id, uint8_t *data, uint16_t data_len, uint8_t *cmd_data_gen)
{
    cmd_data_gen[0] = LCB_CMD_SEND_BYTE;
    cmd_data_gen[1] = 0xFF;
    cmd_data_gen[2] = command_id & 0x00FF;
    cmd_data_gen[3] = data_len & 0x00FF;

    if(data_len > 0) {
        memcpy(&cmd_data_gen[4], data, data_len);
    }
    cmd_data_gen[data_len + 4] = mid_lcb_cmd_get_crc8(cmd_data_gen, data_len + 4);

    return data_len + 5;
}

__attribute__((weak)) void hal_lcb_cmd_send(uint8_t *data, uint16_t data_len) {}
void mid_lcb_cmd_send(uint16_t cmd_id, uint8_t *data, uint16_t data_len)
{
    uint8_t cmd_buffer[256 + 9];
    uint16_t cmd_len = mid_lcb_cmd_gen(cmd_id, data, data_len, cmd_buffer);

#ifdef LCB_CMD_DEBUG
    mid_log_print(LOG_INFO, "LCB cmd tx : ");
    for(int i = 0; i < cmd_len; i++) {
        printf("%02X ", cmd_buffer[i]);
    }
    printf("\r\n");
#endif
    hal_lcb_cmd_send(cmd_buffer, cmd_len);
}

void mid_lcb_send_init_cmd(void)
{
    mid_lcb_cmd_send(LCB_CMD_INIT, NULL, 0);
}

void mid_lcb_send_version_cmd(void)
{
    mid_lcb_cmd_send(LCB_CMD_VERSION, NULL, 0);
}

void mid_lcb_send_erp_cmd(uint8_t data)
{
    uint8_t cmd_data[1];
    cmd_data[0] = data;
    mid_lcb_cmd_send(LCB_CMD_ERP, cmd_data, 1);
}

