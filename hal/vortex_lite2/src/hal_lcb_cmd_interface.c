#include <stdbool.h>
#include <string.h>
#include "hal_gpio.h"
#include "hal_lcb_cmd_interface.h"
#include "hal_power.h"
#include "hal_uart.h"
#include "hal_systick.h"
#include "mid_comm_monitor_led.h"
#include "mid_erp_flow.h"
#include "mid_lcb_cmd.h"


void hal_lcb_leave_erp_success_handler(void)
{
    hal_set_soc_power_on();
}

void hal_lcb_erp_cmd_fail_handler(void)
{
    hal_set_soc_mcu_reset();
}

void hal_lcb_cmd_handler(const LCB_CMD *p_lcb_cmd)
{
    if(mid_erp_status_get() == LCB_WAKE_UP_STATUS) {
        if(p_lcb_cmd->cmd == LCB_CMD_ERP && p_lcb_cmd->cmd_data_buffer[0] == 0x01) {
            mid_erp_status_set(LEAVE_ERP_READY_STATUS);
            hal_lcb_leave_erp_success_handler();
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

void hal_lcb_tx_pin_mode_switch(bool gpio_mode)
{
    if(gpio_mode) {
        hal_output_gpio_init(MCU_UART3_TXD_GPIO);
    }
    else {
        gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_10);
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    }
}

void hal_lcb_wakeup_init(void)
{
    hal_lcb_tx_pin_mode_switch(true);
    hal_delay_1ms(1);
    hal_gpio_set(MCU_UART3_RTS_GPIO, GPIO_HIGH);
    hal_delay_1ms(1);
    hal_gpio_set(MCU_UART3_TXD_GPIO, GPIO_LOW);
    hal_delay_1ms(400);
    hal_gpio_set(MCU_UART3_TXD_GPIO, GPIO_HIGH);
    hal_delay_1ms(4);
    hal_gpio_set(MCU_UART3_RTS_GPIO, GPIO_LOW);
    hal_lcb_tx_pin_mode_switch(false);
    hal_delay_1ms(1);
}

void hal_lcb_rs485_ctrl_tx(bool enable)
{
    if(enable) {
        hal_gpio_set(MCU_UART3_RTS_GPIO, GPIO_HIGH);
    }
    else {
        hal_gpio_set(MCU_UART3_RTS_GPIO, GPIO_LOW);
    }
}

void hal_lcb_cmd_send(uint8_t *data, uint16_t data_len)
{
    hal_lcb_rs485_ctrl_tx(true);
    hal_delay_1ms(1);
    hal_uart_send(UART_ID_3, data, data_len);
    hal_delay_1ms(2);
    hal_lcb_rs485_ctrl_tx(false);
}

extern uint8_t uart3_rxbuffer[256];
extern uint16_t uart3_rx_count;
uint16_t hal_get_soc_lcb_last_cmd(uint8_t *cmd_buffer)
{
    memcpy(cmd_buffer, uart3_rxbuffer, uart3_rx_count);
    return uart3_rx_count;
}