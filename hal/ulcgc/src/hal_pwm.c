#ifdef TONE_CONTROL_BY_PWM

#include "gd32f5xx.h"
#include "mid_log.h"
#include "hal_pwm.h"

void pwm_pa11_init(void)
{
    /*gpio config */
    rcu_periph_clock_enable(RCU_GPIOA);

    /*Configure PA11(TIMER0 CH3) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_11);

    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL2);

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 23999;
    timer_initpara.period            = 9999;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    timer_init(TIMER0, &timer_initpara);

    /* CH3 configuration in PWM mode */
    timer_ocintpara.ocpolarity       = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate      = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity      = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate     = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate      = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate     = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocintpara);

    /* CH3 configuration in PWM mode1, duty cycle 5% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 0);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    /* primary output enable */
    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
}

void pwm_pa11_deinit(void)
{
    timer_deinit(TIMER0);
}

void hal_pwm_set_pulse(enum PWM_PIN_ID pin_id, uint32_t pulse)
{
    switch(pin_id) {
    case PWM_PA11:
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, pulse);
        break;
    }
}

void hal_pwm_set_enable(enum PWM_PIN_ID pin_id, bool enable)
{
    switch(pin_id) {
    case PWM_PA11:
        if(enable) {
            timer_enable(TIMER0);
        }
        else {
            timer_disable(TIMER0);
        }
        break;
    }
}

void hal_pwm_init(void)
{
    pwm_pa11_init(); //BUZER PWM
}

void hal_pwm_deinit(void)
{
    pwm_pa11_deinit();
}

#endif