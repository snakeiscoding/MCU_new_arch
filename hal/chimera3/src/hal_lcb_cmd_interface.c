#include <stdbool.h>
#include <string.h>
#include "hal_gpio.h"
#include "hal_lcb_cmd_interface.h"
#include "hal_uart.h"
#include "hal_systick.h"
#include "mid_comm_monitor_led.h"
#include "mid_erp_flow.h"
#include "mid_lcb_cmd.h"

void hal_lcb_cmd_handler(const LCB_CMD *p_lcb_cmd)
{
    if(mid_erp_status_get() == LCB_WAKE_UP_STATUS) {
        if(p_lcb_cmd->cmd == LCB_CMD_ERP && p_lcb_cmd->cmd_data_buffer[0] == 0x01) {
            mid_erp_status_set(LEAVE_ERP_READY_STATUS);
        }
    }
    else if(mid_erp_status_get() == LCB_ENTER_ERP_STATUS) {
        if(p_lcb_cmd->cmd == LCB_CMD_ERP && p_lcb_cmd->cmd_data_buffer[0] == 0x01) {
            mid_erp_status_set(SOC_POWER_OFF_STATUS);
        }
    }
    else if(mid_erp_status_get() == LCB_INIT_STATUS) {
        if(p_lcb_cmd->cmd == LCB_CMD_INIT) {
            mid_erp_status_set(LCB_ENTER_ERP_STATUS);
        }
    }

    if(mid_get_mcu_lcb_handshake_status() == MCU_LCB_COMM_TEST_STATUS) {
        if(p_lcb_cmd->cmd == LCB_CMD_VERSION) {
            mid_set_mcu_lcb_handshake_success();
        }
    }
}

void hal_lcb_wakeup_init(void)
{
    hal_gpio_set(LCB_RS485_WRITE_GPIO, GPIO_HIGH);
    hal_delay_1ms(1);
    hal_gpio_set(LCB_RS485_TX_GPIO, GPIO_LOW);
    hal_delay_1ms(400);
    hal_gpio_set(LCB_RS485_TX_GPIO, GPIO_HIGH);
    hal_delay_1ms(4);
    hal_gpio_set(LCB_RS485_WRITE_GPIO, GPIO_LOW);
}

void hal_lcb_rs485_ctrl_tx(bool enable)
{
    if(enable) {
        hal_gpio_set(LCB_RS485_WRITE_GPIO, GPIO_HIGH);
    }
    else {
        hal_gpio_set(LCB_RS485_WRITE_GPIO, GPIO_LOW);
    }
}

void hal_lcb_cmd_send(uint8_t *data, uint16_t data_len)
{
    hal_lcb_rs485_ctrl_tx(true);
    hal_delay_1ms(1);
    hal_uart_send(UART_ID_2, data, data_len);
    hal_delay_1ms(2);
    hal_lcb_rs485_ctrl_tx(false);
}