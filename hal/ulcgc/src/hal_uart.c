#include "gd32f5xx_gpio.h"
#include "hal_adc.h"
#include "hal_power.h"
#include "hal_uart.h"

#define USART0_RDATA_ADDRESS    ((uint32_t)&USART_DATA(USART0))
#define USART1_RDATA_ADDRESS    ((uint32_t)&USART_DATA(USART1))
#define UART3_RDATA_ADDRESS     ((uint32_t)&USART_DATA(UART3))
#define UART6_RDATA_ADDRESS     ((uint32_t)&USART_DATA(UART6))

uint8_t usart0_rxbuffer[1024];
uint8_t usart1_rxbuffer[256];
uint8_t uart3_rxbuffer[256];
uint16_t uart3_rx_count = 0;
uint8_t uart6_rxbuffer[64];

static void usart0_nvic_config(void)
{
	nvic_irq_enable(USART0_IRQn, 0, 0);
}

static void usart0_dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1, DMA_CH2);

    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr = (uint32_t)usart0_rxbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.number = 1024;
    dma_init_struct.periph_addr = USART0_RDATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH2, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH2);
    dma_channel_subperipheral_select(DMA1, DMA_CH2, DMA_SUBPERI4);
    /* enable DMA1 channel2 */
    dma_channel_enable(DMA1, DMA_CH2);
}

void usart0_init(void)
{
    usart0_nvic_config();
    usart0_dma_config();
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
	/* enable USART clock */
	rcu_periph_clock_enable(RCU_USART0);
	/* connect port to USARTx_Tx */
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
	/* connect port to USARTx_Rx */
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);
	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	/* configure USART Rx as alternate function push-pull */
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

	/* configure USART */
	usart_deinit(USART0);
	usart_baudrate_set(USART0, 115200U);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_dma_receive_config(USART0, USART_DENR_ENABLE);
	usart_enable(USART0);
	usart_interrupt_enable(USART0, USART_INT_IDLE);
}

void usart0_deinit(void)
{
	usart_deinit(USART0);
	gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_10);
}

static void usart1_nvic_config(void)
{
	nvic_irq_enable(USART1_IRQn, 0, 0);
}

static void usart1_dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA0);
    dma_deinit(DMA0, DMA_CH5);

	dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
	dma_init_struct.memory0_addr = (uint32_t)usart1_rxbuffer;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.number = 256;
	dma_init_struct.periph_addr = USART1_RDATA_ADDRESS;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_single_data_mode_init(DMA0, DMA_CH5, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH5);
    dma_channel_subperipheral_select(DMA0, DMA_CH5, DMA_SUBPERI4);
    /* enable DMA0 channel5 */
    dma_channel_enable(DMA0, DMA_CH5);
}

void usart1_init(void)
{
    usart1_nvic_config();
    usart1_dma_config();
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);
    /* connect port to UARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2);
    /* connect port to UARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_3);
    /* configure UART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    /* configure UART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* configure UART */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_dma_receive_config(USART1, USART_DENR_ENABLE);
    usart_enable(USART1);
    usart_interrupt_enable(USART1, USART_INT_IDLE);
}

void usart1_deinit(void)
{
    usart_deinit(USART1);
}

void usart2_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable UART2 clock */
    rcu_periph_clock_enable(RCU_USART2);
    /* configure the UART2 TX pin and UART2 RX pin */
    gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_10);
    gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_11);

    /* configure USART0 TX as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* configure USART0 RX as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* UART2 configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2, 2000000U);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_enable(USART2);
}

void usart2_deinit(void)
{
    usart_deinit(USART2);
}

static void uart3_nvic_config(void)
{
    nvic_irq_enable(UART3_IRQn, 0, 0);
}

static void uart3_dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA0);
    dma_deinit(DMA0, DMA_CH2);
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr = (uint32_t)uart3_rxbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.number = 256;
    dma_init_struct.periph_addr = UART3_RDATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH2, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_channel_subperipheral_select(DMA0, DMA_CH2, DMA_SUBPERI4);
    /* enable DMA0 channel2 */
    dma_channel_enable(DMA0, DMA_CH2);
}

void uart3_init(void)
{
    uart3_nvic_config();
    uart3_dma_config();
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART3);
    /* connect port to UARTx_Tx */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_10);
    /* connect port to UARTx_Rx */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_11);
    /* configure UART Tx as alternate function push-pull */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* configure UART Rx as alternate function push-pull */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* configure UART */
    usart_deinit(UART3);
    usart_baudrate_set(UART3, 9600U);
    usart_receive_config(UART3, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);
    usart_dma_receive_config(UART3, USART_DENR_ENABLE);
    usart_enable(UART3);
    usart_interrupt_enable(UART3, USART_INT_IDLE);
}

void uart3_deinit(void)
{
	usart_deinit(UART3);
}

static void uart6_nvic_config(void)
{
    nvic_irq_enable(UART6_IRQn, 0, 0);
}

static void uart6_dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA0);

    dma_deinit(DMA0, DMA_CH3);

    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr = (uint32_t)uart6_rxbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.number = 64;
    dma_init_struct.periph_addr = UART6_RDATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH3, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_channel_subperipheral_select(DMA0, DMA_CH3, DMA_SUBPERI5);
    /* enable DMA0 channel3 */
    dma_channel_enable(DMA0, DMA_CH3);
}

void uart6_init(void)
{
    uart6_nvic_config();
    uart6_dma_config();

    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART6);
    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOE, GPIO_AF_8, GPIO_PIN_8);
    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOE, GPIO_AF_8, GPIO_PIN_7);
    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* configure USART */
    usart_deinit(UART6);
    usart_baudrate_set(UART6, 115200U);
    usart_receive_config(UART6, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART6, USART_TRANSMIT_ENABLE);
    usart_dma_receive_config(UART6, USART_DENR_ENABLE);
    usart_enable(UART6);
    usart_interrupt_enable(UART6, USART_INT_IDLE);
}

void uart6_deinit(void)
{
    usart_deinit(UART6);
    gpio_mode_set(GPIOE, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_8);
}

static void hal_usart0_send(uint8_t *data, uint16_t len)
{
    for(int i = 0; i < len; i++) {
        usart_data_transmit(USART0, data[i]);
        while (RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }
}

static void hal_usart1_send(uint8_t *data, uint16_t len)
{
    for(int i = 0; i < len; i++) {
        usart_data_transmit(USART1, data[i]);
        while (RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    }
}

static void hal_usart2_send(uint8_t *data, uint16_t len)
{
    for(int i = 0; i < len; i++) {
        usart_data_transmit(USART2, data[i]);
        while (RESET == usart_flag_get(USART2, USART_FLAG_TBE));
    }
}

static void hal_uart3_send(uint8_t *data, uint16_t len)
{
    for(int i = 0; i < len; i++) {
        usart_data_transmit(UART3, data[i]);
        while (RESET == usart_flag_get(UART3, USART_FLAG_TBE));
    }
}

static void hal_uart6_send(uint8_t *data, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        usart_data_transmit(UART6, data[i]);
        while (RESET == usart_flag_get(UART6, USART_FLAG_TBE));          
    }
}

void hal_uart_send(enum UART_ID uart_id, uint8_t *data, uint8_t len)
{
    if(hal_get_power_mode() != POWER_MODE_ACTIVE)
    {
        return;
    }
    switch(uart_id) {
    case UART_ID_0:
        hal_usart0_send(data, len);
        break;
    case UART_ID_1:
        hal_usart1_send(data, len);
        break;
    case UART_ID_2:
        hal_usart2_send(data, len);
        break;
    case UART_ID_3:
        hal_uart3_send(data, len);
        break;
    case UART_ID_6:
        hal_uart6_send(data, len);
        break;
    }
}

void hal_uart_init(void)
{
    usart0_init();
    usart1_init();
    usart2_init();
    uart3_init();
    uart6_init();
}

void hal_uart_deinit(void)
{
    usart0_deinit();
    usart1_deinit();
    //usart2_deinit();
    uart3_deinit();
    uart6_deinit();
}