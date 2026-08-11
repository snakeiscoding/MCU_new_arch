#ifndef HAL_LCB_CMD_INTERFACE_H__
#define HAL_LCB_CMD_INTERFACE_H__

#include "mid_lcb_cmd.h" 

void hal_lcb_leave_erp_success_handler(void);
void hal_lcb_erp_cmd_fail_handler(void);
void hal_lcb_cmd_handler(const LCB_CMD *p_lcb_cmd);
void hal_lcb_wakeup_init(void);
void hal_lcb_cmd_send(uint8_t *data, uint16_t data_len);
uint16_t hal_get_soc_lcb_last_cmd(uint8_t *cmd_buffer);

#endif //HAL_LCB_CMD_INTERFACE_H__