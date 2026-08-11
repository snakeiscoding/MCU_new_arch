#include "gd32f5xx.h"
#include "hal_pwm.h"

void pwm_pa7_init(void)
{
    /*gpio config */
    rcu_periph_clock_enable(RCU_GPIOA);

    /*Configure PA7(TIMER2 CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_7);

    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

    timer_deinit(TIMER2);

    /* TIMER2 configuration */
    timer_initpara.prescaler = 199;
    timer_initpara.period = 99;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    /* CH1 configuration in PWM mode */
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER2, TIMER_CH_1, &timer_ocintpara);

    /* CH1 configuration in PWM mode1, duty cycle 5% */
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, 0);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
}

void pwm_pa7_deinit(void)
{
    timer_deinit(TIMER2);    
}

uint32_t rgb_led_pwm_buf[256];
void pwm_rgb_led_init(void)
{
    timer_deinit(TIMER7);
    
    /* TIMER7 configuration */
	timer_parameter_struct timer_initpara;
	timer_initpara.prescaler         = 83;
	timer_initpara.period            = 99;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;

    rcu_periph_clock_enable(RCU_TIMER7);

	timer_init(TIMER7, &timer_initpara);
	timer_dma_enable(TIMER7, TIMER_DMA_UPD);
	timer_enable(TIMER7);
	
	rcu_periph_clock_enable(RCU_DMA1);

	dma_single_data_parameter_struct dma_init_struct;
	dma_deinit(DMA1, DMA_CH1);
	dma_channel_subperipheral_select(DMA1, DMA_CH1, DMA_SUBPERI7);

	dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
	dma_init_struct.memory0_addr = (uint32_t)rgb_led_pwm_buf;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.periph_addr = (uint32_t)&GPIO_BOP(GPIOA);
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
	dma_init_struct.number = 256;
	dma_init_struct.priority = DMA_PRIORITY_HIGH;
	dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_ENABLE;

	dma_single_data_mode_init(DMA1, DMA_CH1, &dma_init_struct);
	dma_circulation_enable(DMA1, DMA_CH1);
	dma_channel_enable(DMA1, DMA_CH1);
}

void pwm_rgb_led_deinit(void)
{
    timer_deinit(TIMER7);
}

void hal_pwm_set_pulse(enum PWM_PIN_ID pin_id, uint32_t pulse)
{
    switch(pin_id) {
    case PWM_PA7:
        timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, pulse);
        break;
    }
}

void hal_pwm_set_enable(enum PWM_PIN_ID pin_id, bool enable)
{
    switch(pin_id) {
    case PWM_PA7:
        if(enable) {
            timer_enable(TIMER2);
        }
        else {
            timer_disable(TIMER2);
        }
        break;
    }
}

void hal_pwm_init(void)
{
    pwm_pa7_init(); //FAN PWM
    pwm_rgb_led_init(); //RGB LED PWM
}

void hal_pwm_deinit(void)
{
    pwm_pa7_deinit();
    pwm_rgb_led_deinit();
}
