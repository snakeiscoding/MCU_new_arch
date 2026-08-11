#include "hal_adc.h"
#include "hal_uart.h"
#include "hal_stm32f4xx_it.h"
#include "stm32f4xx.h"

UART_HandleTypeDef	Uart1Handle, Uart2Handle, Uart3Handle;

#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 

#define USART1_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART1_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USART1 Pins */
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA  
#define USART1_TX_AF                     GPIO_AF7_USART1
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA 
#define USART1_RX_AF                     GPIO_AF7_USART1

/* Definition for USART2 clock resources */
#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USART2_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USART2_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USART2 Pins */
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_GPIO_PORT              GPIOA  
#define USART2_TX_AF                     GPIO_AF7_USART2
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_GPIO_PORT              GPIOA 
#define USART2_RX_AF                     GPIO_AF7_USART2

/* Definition for USART3 clock resources */
#define USART3_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
#define USART3_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 

#define USART3_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USART3_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USART3 Pins */
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_TX_GPIO_PORT              GPIOB  
#define USART3_TX_AF                     GPIO_AF7_USART3
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_RX_GPIO_PORT              GPIOB 
#define USART3_RX_AF                     GPIO_AF7_USART3

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef  GPIO_InitStruct;
    if(huart->Instance == USART1)
	{
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		USART1_TX_GPIO_CLK_ENABLE();
		USART1_RX_GPIO_CLK_ENABLE();
		/* Enable USART2 clock */
		USART1_CLK_ENABLE(); 
		
		/*##-2- Configure peripheral GPIO ##########################################*/  
		/* UART TX GPIO pin configuration  */
		GPIO_InitStruct.Pin       = USART1_TX_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = USART1_TX_AF;
		HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);
			
		/* UART RX GPIO pin configuration  */
		GPIO_InitStruct.Pin = USART1_RX_PIN;
		GPIO_InitStruct.Alternate = USART1_RX_AF;
		HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);
			
		/*##-3- Configure the NVIC for UART ########################################*/
		/* NVIC for USART1 */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
        __HAL_UART_ENABLE_IT(&Uart1Handle, UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(&Uart1Handle, UART_IT_IDLE);
	}

    if(huart->Instance == USART2)
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        /* Enable USART2 clock */
        USART2_CLK_ENABLE(); 
		
        /*##-2- Configure peripheral GPIO ##########################################*/  
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = USART2_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = USART2_TX_AF;
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);
			
        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);
		
        /*##-3- Configure the NVIC for UART ########################################*/
        /* NVIC for USART2 */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        __HAL_UART_ENABLE_IT(&Uart2Handle, UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(&Uart2Handle, UART_IT_IDLE);
	}

	if(huart->Instance == USART3)
	{
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		USART3_TX_GPIO_CLK_ENABLE();
		USART3_RX_GPIO_CLK_ENABLE();
		/* Enable USART3 clock */
		USART3_CLK_ENABLE(); 
		
		/*##-2- Configure peripheral GPIO ##########################################*/  
		/* UART TX GPIO pin configuration  */
		GPIO_InitStruct.Pin       = USART3_TX_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = USART3_TX_AF;
		HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);
			
		/* UART RX GPIO pin configuration  */
		GPIO_InitStruct.Pin = USART3_RX_PIN;
		GPIO_InitStruct.Alternate = USART3_RX_AF;
		HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);
		
		/*##-3- Configure the NVIC for UART ########################################*/
		/* NVIC for USART3 */
		HAL_NVIC_SetPriority(USART3_IRQn, 0, 3);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
}

void usart1_init(void)
{
    Uart1Handle.Instance          = USART1;
    Uart1Handle.Init.BaudRate     = 115200;
    Uart1Handle.Init.WordLength   = UART_WORDLENGTH_8B;
    Uart1Handle.Init.StopBits     = UART_STOPBITS_1;
    Uart1Handle.Init.Parity       = UART_PARITY_NONE;
    Uart1Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    Uart1Handle.Init.Mode         = UART_MODE_TX_RX;
    Uart1Handle.Init.OverSampling = UART_OVERSAMPLING_16;
		
	if(HAL_UART_Init(&Uart1Handle) != HAL_OK) {
        Error_Handler();
    }
}

void usart2_init(void)
{	
    Uart2Handle.Instance          = USART2;
    Uart2Handle.Init.BaudRate     = 9600;
    Uart2Handle.Init.WordLength   = UART_WORDLENGTH_8B;
    Uart2Handle.Init.StopBits     = UART_STOPBITS_1;
    Uart2Handle.Init.Parity       = UART_PARITY_NONE;
    Uart2Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    Uart2Handle.Init.Mode         = UART_MODE_TX_RX;
    Uart2Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if(HAL_UART_Init(&Uart2Handle) != HAL_OK) {
        Error_Handler();
    }
}

void usart3_init(void)
{
	Uart3Handle.Instance          = USART3;
	Uart3Handle.Init.BaudRate     = 115200;
	Uart3Handle.Init.WordLength   = UART_WORDLENGTH_8B;
	Uart3Handle.Init.StopBits     = UART_STOPBITS_1;
	Uart3Handle.Init.Parity       = UART_PARITY_NONE;
	Uart3Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	Uart3Handle.Init.Mode         = UART_MODE_TX_RX;
	Uart3Handle.Init.OverSampling = UART_OVERSAMPLING_16;
		
	if(HAL_UART_Init(&Uart3Handle) != HAL_OK) {
		Error_Handler();
	}
}

static void hal_usart1_send(uint8_t *data, uint16_t len)
{
	HAL_UART_Transmit(&Uart1Handle, data, len, 10);
}

static void hal_usart2_send(uint8_t *data, uint16_t len)
{
	HAL_UART_Transmit(&Uart2Handle, data, len, 10);
}

static void hal_usart3_send(uint8_t *data, uint16_t len)
{
	HAL_UART_Transmit(&Uart3Handle, data, len, 10);
}

void hal_uart_send(enum UART_ID uart_id, uint8_t *data, uint8_t len)
{
	switch(uart_id) {
	case UART_ID_0:
		break;
	case UART_ID_1:
        hal_usart1_send(data, len);
		break;
	case UART_ID_2:
		hal_usart2_send(data, len);
		break;
	case UART_ID_3: 
		hal_usart3_send(data, len);
		break;
	}
}

void hal_uart_init()
{
    usart1_init();
    usart2_init();
	usart3_init();
}