/*!
    \file    gd32f5xx_it.c
    \brief   main interrupt service routines

    \version 2024-12-20, V1.2.0, firmware for GD32F5xx
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/
#include <stdio.h>
#include "app_soc_cmd_event_func.h"
#include "drv_usbd_int.h"
#include "hal_gd32f5xx_it.h"
#include "hal_gpio.h"
#include "hal_key_handler.h"
#include "hal_power.h"
#include "hal_systick.h"
#include "mid_bump_sensor.h"
#include "mid_can_cmd.h"
#include "mid_comm_monitor_led.h"
#include "mid_fan.h"
#include "mid_lcb_cmd.h"
#include "mid_log.h"
#include "mid_scheduler.h"
#include "mid_soc_cmd.h"

/*
#include "main.h"
#include "gd32f527i_eval.h"
#include "drv_led.h"
#include "csafe_interface.h"
#include "user_rs485.h"
#include "user_speaker.h"
#include "user_erp.h"
#include "user_timer.h"
#include "manufacturing_test.h"
#include "user_app.h"
#include "drv_led.h"*/


extern volatile uint64_t time_1ms;
extern usb_core_driver cdc_acm;

extern void usb_timer_irq(void);

/* local function prototypes ('static') */
static void resume_mcu_clk(void);

#define MULTI_ECC_ERROR_HANDLE(s)    do{}while(1)
#define SINGLE_ECC_ERROR_HANDLE(s)   do{}while(1)

// #define UART_LOOPBACK_DEBUG

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
//     if((SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME0)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME1)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME2)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME3)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME4)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME5)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME6))) {
//         MULTI_ECC_ERROR_HANDLE("SRAM or FLASH multi-bits non-correction ECC error\r\n");
//     } else {
//         /* if NMI exception occurs, go to infinite loop */
//         /* HXTAL clock monitor NMI error */
//         while(1) {
//         }
//     }
}

/*!
    \brief      this function handles SRAM and Flash single bit ECC non-correction exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SYSCFG_SINGLE_BIT_ECC_ER_IRQHandler(void)
{
//     if((SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE0)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE1)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE2)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE3)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE4)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE5)) ||
//             (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCSE6))) {
//         MULTI_ECC_ERROR_HANDLE("SRAM or FLASH single bit ECC error\r\n");
//     }
}

/*!
    \brief      this function handles FPU exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void FPU_IRQHandler(void)
{
    /* if FPU exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    time_1ms++;
    hal_delay_decrement();
    mid_1ms_tick_handler();
}

/*!
    \brief      this function handles USART interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
extern uint8_t usart0_rxbuffer[];
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)) {
        /* clear IDLE flag */
		usart_data_receive(USART0);
        //user_led_set(led_yellow, 0);
    
        /* number of data received */
        uint16_t rx_count = 1024 - (dma_transfer_number_get(DMA1, DMA_CH2));
#ifdef UART_LOOPBACK_DEBUG
        printf("usart0 rx : ");
        for (int i = 0;i < rx_count; i++) {
            printf("0x%02X ", usart0_rxbuffer[i]);
        }
        printf("\r\n");
#endif
        mid_sched_event_put((void *)usart0_rxbuffer, rx_count, mid_soc_cmd_receive);
        mid_set_soc_mcu_comm_timer();

        /* disable DMA and reconfigure */
        dma_channel_disable(DMA1, DMA_CH2);
        dma_flag_clear(DMA1, DMA_CH2, DMA_FLAG_FTF);
        dma_transfer_number_config(DMA1, DMA_CH2, 1024);
        dma_channel_enable(DMA1, DMA_CH2);
    }
}

extern uint8_t usart1_rxbuffer[256];
void USART1_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) {
        /* clear IDLE flag */
        usart_data_receive(USART1);

        /* number of data received */
        uint8_t rx_count =256 - (dma_transfer_number_get(DMA0, DMA_CH5));
#ifdef UART_LOOPBACK_DEBUG
        printf("usart1 rx : ");
        for (int i = 0;i < rx_count; i++) {
            printf("0x%02X ", usart1_rxbuffer[i]);
        }
        printf("\r\n");
#endif
        mid_sched_event_put((void *)usart1_rxbuffer, rx_count, mid_bump_sensor_response_to_soc);

        /* disable DMA and reconfigure */
        dma_channel_disable(DMA0, DMA_CH5);
        dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_FTF);
        dma_transfer_number_config(DMA0, DMA_CH5, 256);
        dma_channel_enable(DMA0, DMA_CH5);
	}
}

extern uint8_t uart3_rxbuffer[256];
extern uint16_t uart3_rx_count;
void UART3_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE)) {
        /* clear IDLE flag */
        usart_data_receive(UART3);

        /* number of data received */
        uart3_rx_count = 256 - (dma_transfer_number_get(DMA0, DMA_CH2));
#ifdef UART_LOOPBACK_DEBUG
        printf("uart3 rx : ");
        for (int i = 0;i < uart3_rx_count; i++) {
            printf("0x%02X ", uart3_rxbuffer[i]);
        }
        printf("\r\n");
#endif
        mid_sched_event_put((void *)uart3_rxbuffer, uart3_rx_count, mid_lcb_cmd_receive);
        mid_set_soc_lcb_comm_timer();
        /*if (user_get_rs485_mode() == 0) {
            if (get_dfu_mode() == 0) {
                app_sched_event_put((void *)uart3_rxbuffer, uart3_rx_count, sch_rs485_rx_handle);
            }      
        }*/

        /* disable DMA and reconfigure */
        dma_channel_disable(DMA0, DMA_CH2);
        dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
        dma_transfer_number_config(DMA0, DMA_CH2, 256);
        dma_channel_enable(DMA0, DMA_CH2);
	}
}

extern uint8_t uart6_rxbuffer[64];
void UART6_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(UART6, USART_INT_FLAG_IDLE)) {
        /* clear IDLE flag */
        usart_data_receive(UART6);

        /* number of data received */
#ifdef UART_LOOPBACK_DEBUG
        uint16_t rx_count = 64 - (dma_transfer_number_get(DMA0, DMA_CH3));
        printf("uart6 rx : ");
        for (int i = 0;i < rx_count; i++) {
            printf("0x%02X ", uart6_rxbuffer[i]);
        }
        printf("\r\n");
#endif

        /* disable DMA and reconfigure */
        dma_channel_disable(DMA0, DMA_CH3);
        dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF);
        dma_transfer_number_config(DMA0, DMA_CH3, 64);
        dma_channel_enable(DMA0, DMA_CH3);
	}
}

extern can_receive_message_struct can_receive_message;
void CAN1_RX0_IRQHandler(void)
{
    can_message_receive(CAN1, CAN_FIFO0, &can_receive_message);
    mid_sched_event_put((void *)&can_receive_message, sizeof(can_receive_message), mid_can_cmd_receive);
}

/*!
    \brief      this function handles timer interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
/*void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}*/
void TIMER1_IRQHandler(void)
{
    usb_timer_irq();
}

#ifdef USE_USB_FS
/*!
    \brief      this function handles USBFS wakeup interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if(cdc_acm.bp.low_power) {
        resume_mcu_clk();
        rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
        rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
        rcu_periph_clock_enable(RCU_USBFS);
        usb_clock_active(&cdc_acm);
    }
    exti_interrupt_flag_clear(EXTI_18);
}

#elif defined(USE_USB_HS)
/*!
    \brief      this function handles USBHS wakeup interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_WKUP_IRQHandler(void)
{
    if(cdc_acm.bp.low_power) {
        resume_mcu_clk();
#ifdef USE_EMBEDDED_PHY
        rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
        rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
#elif defined(USE_ULPI_PHY)
        rcu_periph_clock_enable(RCU_USBHSULPI);
#endif
        rcu_periph_clock_enable(RCU_USBHS);
        usb_clock_active(&cdc_acm);
    }
    exti_interrupt_flag_clear(EXTI_20);
}
#endif /* USE_USBFS */

#ifdef USE_USB_FS
/*!
    \brief      this function handles USBFS global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler(void)
{
    usbd_isr(&cdc_acm);
}
#elif defined(USE_USB_HS)
/*!
    \brief      this function handles USBHS global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_IRQHandler(void)
{
    usbd_isr(&cdc_acm);
}
#endif /* USE_USBFS */

#ifdef USB_HS_DEDICATED_EP1_ENABLED
/*!
    \brief      this function handles EP1_IN interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_EP1_In_IRQHandler(void)
{
    usbd_int_dedicated_ep1in(&cdc_acm);
}

/*!
    \brief      this function handles EP1_OUT interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_EP1_Out_IRQHandler(void)
{
    usbd_int_dedicated_ep1out(&cdc_acm);
}
#endif /* USB_HS_DEDICATED_EP1_ENABLED */

/*!
    \brief      resume mcu clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HXTAL */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HXTAL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)) {
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)) {
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLLP);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLLP != rcu_system_clock_source_get()) {
    }
}

void EXTI0_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(MCU_SW_POSITION_EXTI_LINE)) {
        exti_interrupt_flag_clear(MCU_SW_POSITION_EXTI_LINE);
        /*uint8_t key_state = hal_gpio_get(MCU_SW_POSITION_GPIO);
        printf("e-stop: %d\r\n", key_state);*/
        app_soc_cmd_event_estop_func();
    }
}

void EXTI1_IRQHandler(void) {}

void EXTI2_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(MCU_RECOVERY_EXTI_LINE)) {
        exti_interrupt_flag_clear(MCU_RECOVERY_EXTI_LINE);
        mid_sched_event_put((void *)NULL, 0, hal_mcu_recovery_key_it);
    }
}

void EXTI3_IRQHandler(void) {}

void EXTI5_9_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(FAN_RPM_EXTI_LINE)) {
        exti_interrupt_flag_clear(FAN_RPM_EXTI_LINE);
        // mid_log_print(LOG_VERBOSE, "Fan rpm\r\n");
        mid_calcu_fan_rpm();
    }
    if(RESET != exti_interrupt_flag_get(ERP_IR_INT_EXTI_LINE)) {
        exti_interrupt_flag_clear(ERP_IR_INT_EXTI_LINE);
        mid_log_print(LOG_VERBOSE, "IR trigger\r\n");
    }
}

void EXTI10_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(SOC_RESET_EXTI_LINE)) {
        exti_interrupt_flag_clear(SOC_RESET_EXTI_LINE);
        mid_log_print(LOG_VERBOSE, "SOC reset\r\n");
    }
    if(RESET != exti_interrupt_flag_get(MCU_KEY_INT_EXTI_LINE)) {
        exti_interrupt_flag_clear(MCU_KEY_INT_EXTI_LINE);
        hal_set_erp_wakeup_flag();
        mid_sched_event_put((void *)NULL, 0, hal_keypad_it);
    }
}
    
