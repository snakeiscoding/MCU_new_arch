#ifndef MID_LCB_CMD_H__
#define MID_LCB_CMD_H__

#include <stdint.h>

typedef struct LCB_CMD {
	uint16_t cmd;
    uint16_t cmd_data_len;
    uint8_t cmd_data_buffer[256];
} LCB_CMD;

#define LCB_CMD_INIT    ((uint8_t)0x70)
#define LCB_CMD_VERSION ((uint8_t)0x73)
#define LCB_CMD_ERP     ((uint8_t)0x7A)

#define LCB_LEAVE_ERP ((uint8_t)0x00)
#define LCB_ENTER_ERP ((uint8_t)0xFF)

void mid_lcb_cmd_receive(void *p_event_data, uint16_t event_size);
void mid_lcb_wakeup_init(void);
void mid_lcb_send_init_cmd(void);
void mid_lcb_send_version_cmd(void);
void mid_lcb_send_erp_cmd(uint8_t data);
#endif //MID_LCB_CMD_H__