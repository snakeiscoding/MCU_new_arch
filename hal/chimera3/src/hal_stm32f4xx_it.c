#include <stdio.h>
#include "hal_erp_handler.h"
#include "hal_gpio.h"
#include "hal_ir.h"
#include "hal_stm32f4xx_it.h"
#include "hal_systick.h"
#include "mid_fan.h"
#include "mid_lcb_cmd.h"
#include "mid_scheduler.h"
#include "mid_soc_cmd.h"
#include "stm32f4_discovery.h"

void NMI_Handler(void)
{
    /* Go to infinite loop when NMI Handler exception occurs */
    printf("Into the NMI_Handler function.\r\n");
    while(1) {
    }
}

/**
* @brief  This function handles Hard Fault exception.
* @param  None
* @retval None
*/
void HardFault_Handler(void)
{
    //_pINFOCTL pInfoCtl = (_pINFOCTL)SRAM2_ADDR_PARTITION;
        
        //memset((uCHAR *)pInfoCtl, 0xff, 0x4000);
    /* Go to infinite loop when Hard Fault exception occurs */
    while(1) {
            /*printf("Into the HardFault_Handler function.\r\n");
            pInfoCtl->AppErrorCnt++;
            
            MemoryMoveFromSourToDest((uint32_t)pInfoCtl, INFO_FLASH_PARTITION);
            MemoryMoveFromSourToDest(INFO_FLASH_PARTITION, INFO_BACK_UP_FLASH_PARTITION);
            printf("Update the info memory is done.\r\n");*/
            //while(1);
    }
}

/**
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
*/
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    printf("Into the MemManage_Handler function.\r\n");
    while(1) {
    }
}

/**
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
*/
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    printf("Into the BusFault_Handler function.\r\n");
    while(1) {
    }
}

/**
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
*/
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    printf("Into the UsageFault_Handler function.\r\n");
    while(1) {
    }
}

/**
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
*/
void DebugMon_Handler(void)
{
    /* Go to infinite loop when Debug Monitor exception occurs */
    printf("Into the DebugMon_Handler function.\r\n");
    while(1) {
    }
}

/**
* @brief  This function handles PendSVC exception.
* @param  None
* @retval None
*/
void PendSV_Handler(void)
{
    /* Go to infinite loop when PendSVC exception occurs */
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
    printf("Into the PendSVC function.\r\n");
    //while(1) {
    //}
}

extern UART_HandleTypeDef Uart1Handle, Uart2Handle, Uart3Handle;

uint8_t usart1_rxbuffer[1024];
volatile uint16_t usart1_rx_count = 0;
void USART1_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&Uart1Handle, UART_FLAG_RXNE) != RESET) {
        usart1_rxbuffer[usart1_rx_count] = (uint8_t)(Uart1Handle.Instance->DR & 0x00FF);
        usart1_rx_count++;
    }

    if(__HAL_UART_GET_FLAG(&Uart1Handle, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&Uart1Handle);
        /*for (int i = 0;i < usart1_rx_count;i ++) {
            printf("%02X ", usart1_rxbuffer[i]);
        }
        printf("\r\n");*/
        mid_sched_event_put((void *)usart1_rxbuffer, usart1_rx_count, mid_soc_cmd_receive);
        usart1_rx_count = 0; 
    }

    HAL_UART_IRQHandler(&Uart1Handle);
}

uint8_t usart2_rxbuffer[256];
volatile uint16_t usart2_rx_count = 0;
void USART2_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&Uart2Handle, UART_FLAG_RXNE) != RESET) {
        usart2_rxbuffer[usart2_rx_count] = (uint8_t)(Uart2Handle.Instance->DR & 0x00FF);
        usart2_rx_count++;
    }

    if(__HAL_UART_GET_FLAG(&Uart2Handle, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&Uart2Handle);
        /*for (int i = 0;i < usart2_rx_count;i ++) {
            printf("%02X ", usart2_rxbuffer[i]);
        }
        printf("\r\n");*/
        mid_sched_event_put((void *)usart2_rxbuffer, usart2_rx_count, mid_lcb_cmd_receive);
        usart2_rx_count = 0; 
    }

    HAL_UART_IRQHandler(&Uart2Handle);
}

void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&Uart3Handle);
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void Error_Handler(void)
{
    /* Turn LED5 on */
    printf("Into the Error_Handler!\r\n\r\n");
    while(1) {
        HAL_Delay(500);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == FAN_RPM_GPIO_PIN) {
        mid_calcu_fan_rpm();
    }
    /* The keypad sense lines are only EXTI while the MCU is in STOP mode -
     * hal_erp_handler.c rearms them as the wake source before sleeping and
     * hal_system_init() returns them to polled inputs after wake. */
    else if(GPIO_Pin == KEYV_D1_GPIO_PIN || GPIO_Pin == KEYV_D2_GPIO_PIN) {
        hal_erp_wakeup_flag_set();
    }
    else if(GPIO_Pin == IR_IN_GPIO_PIN) {
        hal_ir_rx_edge();
    }
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(FAN_RPM_GPIO_PIN);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(IR_IN_GPIO_PIN);
    HAL_GPIO_EXTI_IRQHandler(KEYV_D1_GPIO_PIN);
    HAL_GPIO_EXTI_IRQHandler(KEYV_D2_GPIO_PIN);
}

/* IR transmit timebase, see hal_ir.c. */
extern TIM_HandleTypeDef Tim7Handle;
void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&Tim7Handle);
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) , for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
//void TIM1_UP_TIM10_IRQHandler(void)
//{
//	HAL_TIM_IRQHandler(&Tim10Handle);
//	BSP_LED_Toggle(LED4);
//}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
/**
*@brief  This function handles PPP interrupt request.
*@param  None
*@retval None
*/
/*void PPP_IRQHandler(void)
{
}*/


/**
* @}
*/ 