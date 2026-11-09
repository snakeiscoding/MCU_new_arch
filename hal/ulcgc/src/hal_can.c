#include <stdio.h>
#include <string.h>
#include "gd32f5xx.h"
#include "hal_can.h"
#include "hal_gpio.h"
#include "mid_log.h"

can_receive_message_struct can_receive_message;
can_trasnmit_message_struct can_transmit_message;
#define DEV_CAN1_SFID   ((uint32_t)0x07AA)
#define DEV_CAN1_EFID   ((uint32_t)0xCCDD)
// #define CAN_LOOPBACK_DEBUG
// #define CAN_LOOPBACK_INTERNAL

void hal_can_config(uint16_t baudrate_value)
{
    can_parameter_struct can_parameter;
    can_filter_parameter_struct can_filter;
    can_interrupt_disable(CAN1, CAN_INT_RFNE0);
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);
    /* initialize CAN register */
    can_deinit(CAN1);

    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = ENABLE;
    can_parameter.trans_fifo_order = DISABLE;
#ifdef CAN_LOOPBACK_INTERNAL
    can_parameter.working_mode = CAN_LOOPBACK_MODE;
#else
    can_parameter.working_mode = CAN_NORMAL_MODE;
#endif

    /* baudrate 1Mbps, sample piont at 80% */
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_7TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_2TQ;

    switch(baudrate_value) {
    case 1000:
        can_parameter.prescaler = 5;
        break;
    case 500:
        can_parameter.prescaler = 10;
        break;
    case 250:
        can_parameter.prescaler = 20;
        break;
    case 125:
        can_parameter.prescaler = 40;
        break;
    case 100:
        can_parameter.prescaler = 50;
        break;
    case 50:
        can_parameter.prescaler = 100;
        break;
    case 20:
        can_parameter.prescaler = 250;
        break;
    default:
        can_parameter.prescaler = 1;
        break;
    }
    /* initialize CAN */
    can_init(CAN1, &can_parameter);

    /* initialize filter */
    can_filter.filter_number = 0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;

    can_filter.filter_number = 15;
    can_filter_init(&can_filter);

    can_fdframe_struct can_fd_parameter;
    can_fd_tdc_struct can_fd_tdc_parameter;
    can_struct_para_init(CAN_FD_FRAME_STRUCT, &can_fd_parameter);
    can_fd_parameter.fd_frame = ENABLE;
    can_fd_parameter.excp_event_detect = ENABLE;
    can_fd_parameter.delay_compensation = ENABLE;
    can_fd_tdc_parameter.tdc_filter = 0x04;
    can_fd_tdc_parameter.tdc_mode = CAN_TDCMOD_CALC_AND_OFFSET;
    can_fd_tdc_parameter.tdc_offset = 0x04;
    can_fd_parameter.p_delay_compensation = &can_fd_tdc_parameter;
    can_fd_parameter.iso_bosch = CAN_FDMOD_ISO;
    can_fd_parameter.esi_mode = CAN_ESIMOD_HARDWARE;
    /* FD mode data baudrate 2Mbps, sample piont at 80% */
    can_fd_parameter.data_resync_jump_width = CAN_BT_SJW_1TQ;
    can_fd_parameter.data_time_segment_1 = CAN_BT_BS1_3TQ;
    can_fd_parameter.data_time_segment_2 = CAN_BT_BS2_1TQ;
    /* CAN-FD data segment prescaler is an independent register (CAN_DBT),
     * not coupled to the nominal-phase prescaler !X intentionally fixed
     * here, not per-case. */
    can_fd_parameter.data_prescaler = 5;
    can_fd_init(CAN1, &can_fd_parameter);

    can_interrupt_enable(CAN1, CAN_INT_RFNE0);
}

void hal_can_init(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* configure CAN1 GPIO */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_12);

    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_13);

    /* configure CAN1 NVIC */
    nvic_irq_enable(CAN1_RX0_IRQn, 1, 0);

    hal_can_config(1000);

#ifdef CAN_LOOPBACK_DEBUG
    can_receive_message_struct test_received_packet;

    test_received_packet.rx_ff = CAN_FF_STANDARD;
    test_received_packet.rx_ft = CAN_FT_DATA;
    test_received_packet.rx_sfid = 0x123;
    test_received_packet.rx_dlen = 4;
    test_received_packet.rx_data[0] = 0xAA;
    test_received_packet.rx_data[1] = 0xBB;
    test_received_packet.rx_data[2] = 0xCC;
    test_received_packet.rx_data[3] = 0xDD;
    test_received_packet.fd_flag = DISABLE;

    hal_can_cmd_send(&test_received_packet, sizeof(test_received_packet));
#endif
}

void hal_can_deinit(void)
{
    hal_gpio_set(CAN_MODE_GPIO, GPIO_LOW);
	nvic_irq_disable(CAN1_RX0_IRQn);
	can_deinit(CAN1);
	can_deinit(CAN0);
}

void hal_can_cmd_send(void *p_event_data, uint16_t event_size)
{
    uint8_t send_buf[128];
    can_receive_message_struct *can_receive_data = p_event_data;

    if(can_receive_data->rx_ff == CAN_FF_STANDARD) {
        send_buf[2] = can_receive_data->rx_sfid;
        send_buf[3] = can_receive_data->rx_sfid >> 8;
        send_buf[4] = can_receive_data->rx_sfid >> 16;
        send_buf[5] = can_receive_data->rx_sfid >> 24;
        mid_log_print(LOG_VERBOSE, "standard frame ID : %X", can_receive_data->rx_sfid);
    }
    else if(can_receive_data->rx_ff == CAN_FF_EXTENDED) {
        send_buf[2] = can_receive_data->rx_efid;
        send_buf[3] = can_receive_data->rx_efid >> 8;
        send_buf[4] = can_receive_data->rx_efid >> 16;
        send_buf[5] = can_receive_data->rx_efid >> 24;
        mid_log_print(LOG_VERBOSE, "extended frame ID : %X", can_receive_data->rx_efid);
    }

    send_buf[6] = can_receive_data->rx_dlen;
    if(can_receive_data->rx_ft == CAN_FT_REMOTE) {
        printf(" remote frame len : %d\r\n", can_receive_data->rx_dlen);
    }
    else if(can_receive_data->rx_ft == CAN_FT_DATA) {
        printf(" data frame len : %d\r\n", can_receive_data->rx_dlen);
    }

    if(can_receive_data->rx_ft == CAN_FT_DATA) {
        memcpy(&send_buf[7], &can_receive_data->rx_data[0], can_receive_data->rx_dlen);
        printf("data : ");
        for(uint8_t i = 0; i < can_receive_data->rx_dlen; i++) {
            printf("%02X ", can_receive_data->rx_data[i]);
        }
        printf("\r\n");
    }

    can_transmit_message.fd_flag = can_receive_data->fd_flag;
    can_transmit_message.tx_sfid = DEV_CAN1_SFID;
    can_transmit_message.tx_efid = DEV_CAN1_EFID;
    can_transmit_message.tx_ff = can_receive_data->rx_ff;
    can_transmit_message.tx_ft = can_receive_data->rx_ft;
    can_transmit_message.tx_dlen = can_receive_data->rx_dlen ;
    memcpy(&can_transmit_message.tx_data[0], &can_receive_data->rx_data[0], can_receive_data->rx_dlen);
    can_message_transmit(CAN1, &can_transmit_message);
}
