#include <stdio.h>
#include <string.h>
#include "app_soc_cmd.h"
#include "hal_soc_cmd_interface.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"

static SOC_CMD m_soc_cmd;

#define SOC_CMD_HEADER_HIGH_BYTE ((uint8_t)0x55)
#define SOC_CMD_HEADER_LOW_BYTE  ((uint8_t)0xAA)

#define SOC_CMD_PARSER_SUCCESS   0
#define SOC_CMD_PARSER_FAIL      1

#define SOC_CMD_DEBUG

uint8_t soc_cmd_crc8_table[16] = {0x00, 0x5A, 0xB4, 0xEE, 0x32, 0x68, 0x86, 0xDC, 0x64, 0x3E, 0xD0, 0x8A, 0x56, 0x0C, 0xE2, 0xB8};
static uint8_t mid_soc_cmd_get_crc8(uint8_t *data, uint16_t len)
{
    uint16_t crc_count;
    uint8_t high_nibble, data_temp, new_crc = 0;

    crc_count = len;
    while (crc_count-- != 0) {
        data_temp = *data;
        high_nibble = new_crc / 16;
        new_crc <<= 4;
        new_crc ^= soc_cmd_crc8_table[high_nibble ^ (data_temp / 16)];
        high_nibble = new_crc / 16;
        new_crc <<= 4;

        new_crc ^= soc_cmd_crc8_table[high_nibble ^ (data_temp & 0x0F)];
        data++;
    }

	return new_crc;
}

int mid_soc_cmd_parser(uint8_t *data, uint16_t data_len)
{
    if(data_len < 9) {
        mid_log_print(LOG_ERROR, "SoC command length too short!\r\n");
        return SOC_CMD_PARSER_FAIL;
    }

    if(!(data[0] == SOC_CMD_HEADER_HIGH_BYTE && data[1] == SOC_CMD_HEADER_LOW_BYTE)) {
        mid_log_print(LOG_ERROR, "SoC command header error!\r\n");
        return SOC_CMD_PARSER_FAIL;
    }

    m_soc_cmd.cmd_data_len = (uint16_t)data[6] | ((uint16_t)data[7] << 8);
    if(m_soc_cmd.cmd_data_len + 9 != data_len) {
        mid_log_print(LOG_ERROR, "SoC command length error!\r\n");
        return SOC_CMD_PARSER_FAIL;
    }

    uint8_t crc8_data = mid_soc_cmd_get_crc8(data, data_len - 1);
#ifdef SOC_CMD_DEBUG
    mid_log_print(LOG_INFO, "SoC command crc : 0x%02X\r\n", crc8_data);
#endif
    if(crc8_data != data[data_len - 1]) {
        mid_log_print(LOG_ERROR, "SoC command crc checksum error!\r\n");
        return SOC_CMD_PARSER_FAIL;
    }

    m_soc_cmd.cmd = (uint16_t)data[2] | ((uint16_t)data[3] << 8);
    memcpy((void *)m_soc_cmd.cmd_data_buffer, (void *)&data[8], m_soc_cmd.cmd_data_len);

    return SOC_CMD_PARSER_SUCCESS;
}

void mid_soc_cmd_receive(void *p_event_data, uint16_t event_size)
{
	uint8_t *data = (uint8_t *)p_event_data;
	uint16_t data_len = event_size;
#ifdef SOC_CMD_DEBUG
    mid_log_print(LOG_INFO, "SoC cmd rx : ");
    for (int i = 0; i < data_len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\r\n");
#endif

    if(mid_soc_cmd_parser(data, data_len) == SOC_CMD_PARSER_SUCCESS) {
        app_soc_cmd_handler(&m_soc_cmd);
    }
}

uint16_t mid_soc_cmd_gen(uint16_t cmd_id, uint16_t cmd_result, uint8_t *data, uint16_t data_len, uint8_t *cmd_data_gen)
{
    cmd_data_gen[0] = SOC_CMD_HEADER_HIGH_BYTE;
    cmd_data_gen[1] = SOC_CMD_HEADER_LOW_BYTE;

    cmd_data_gen[2] = cmd_id & 0x00FF;
    cmd_data_gen[3] = (cmd_id & 0xFF00) >> 8;

    cmd_data_gen[4] = cmd_result  & 0x00FF;;
    cmd_data_gen[5] = (cmd_result & 0xFF00) >> 8;

    cmd_data_gen[6] = data_len & 0x00FF;
    cmd_data_gen[7] = (data_len & 0xFF00) >> 8;

    if(data_len > 0) {
        memcpy(&cmd_data_gen[8], data, data_len);
    }

    cmd_data_gen[data_len + 8] = mid_soc_cmd_get_crc8(cmd_data_gen, data_len + 8);

    return data_len + 9;
}

__attribute__((weak)) void hal_soc_cmd_send(uint8_t *data, uint16_t len) {}
void mid_soc_cmd_send(uint16_t cmd_id, uint16_t cmd_result, uint8_t *data, uint16_t data_len)
{
    uint8_t cmd_buffer[1024 + 9];
    uint16_t cmd_len = mid_soc_cmd_gen(cmd_id, cmd_result, data, data_len, cmd_buffer);

#ifdef SOC_CMD_DEBUG
    mid_log_print(LOG_INFO, "SoC cmd tx : ");
    for(int i = 0; i < cmd_len; i++) {
        printf("%02X ", cmd_buffer[i]);
    }
    printf("\r\n");
#endif
    hal_soc_cmd_send(cmd_buffer, cmd_len);
}