#include <math.h>
#include "gd32f5xx_adc.h"
#include "hal_adc.h"
#include "hal_systick.h"
#include "mid_log.h"

// #define ADC_DEBUG

/* 
 * ADC reference voltage calibration history:
 * 3.35V (initial) -> 3.34V -> 3.37V (current).
 * Final value was optimized to account for voltage drop and measurement accuracy.
 */
#define ADC_VREF  3.37f

#define HW_ID    ADC_CHANNEL_13   /* Hardware version ID   */
#define MCB_V    ADC_CHANNEL_0    /* MCB 12V voltage (PA0) */
#define EXT_V    ADC_CHANNEL_11   /* EXT 12V voltage (PC1) */
#define MCB_I    ADC_CHANNEL_12   /* MCB 12V current (PC2) */
#define EXT_I    ADC_CHANNEL_4    /* EXT 12V current (PA4) */

enum hardware_version hw_id = hw_k1;
enum power_status_enum MCB12V_status = POWER_12V_NO_POWER;
enum power_status_enum EXT12V_status = POWER_12V_NO_POWER;

uint16_t adc_read_channel(uint8_t channel)
{
    /* ADC routine channel config */
    adc_routine_channel_config(ADC0, 0U, channel, ADC_SAMPLETIME_15);
    /* clear conversion flag before enable */
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    /* wait the end of conversion flag */
    while (!adc_flag_get(ADC0, ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    return adc_routine_data_read(ADC0);
}

float read_voltage(uint8_t channel)
{
    uint16_t adc_value = adc_read_channel(channel);
    float v = (float)adc_value / 4096.0f * ADC_VREF;
    return roundf(v * 1000.0f) / 1000.0f;
}

float read_current(uint8_t channel)
{
    float a = read_voltage(channel) / 0.3f;
    return roundf(a * 1000.0f) / 1000.0f;
}

int calculate_12v_status(float adc_voltage)
{
    if(adc_voltage < 0.1f)   return POWER_12V_NO_POWER;
    if(adc_voltage < 2.37f)  return POWER_12V_LOW;
    if(adc_voltage <= 2.78f) return POWER_12V_OK;
    return POWER_12V_HIGH;
}

void hal_hw_voltage_adc_init(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
}

void hal_pa0_voltage_adc_init(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
}

void hal_pc2_current_adc_init(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);
}

void hal_pc1_voltage_adc_init(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
}

void hal_pa4_current_adc_init(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
}

void hal_adc_init(void)
{
    hal_hw_voltage_adc_init();
    hal_pa0_voltage_adc_init();
    hal_pc1_voltage_adc_init(); 
    hal_pc2_current_adc_init();   
    hal_pa4_current_adc_init();

    /* reset ADC */
    adc_deinit();
    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC contineous function disable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1U);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* enable ADC interface */
    adc_enable(ADC0);
    hal_delay_1ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
}

int hal_get_hw_id(void)
{
    float v = read_voltage(HW_ID);
#ifdef ADC_DEBUG
    mid_log_print(LOG_VERBOSE, "HW voltage: %d.%02d\r\n", (int)v, (int)(v * 100) % 100);
#endif
    if((v >= 0)&&(v <= 0.1)) {
        hw_id = hw_k1;
    }
    else if((v > 0.2) && (v <= 0.4)) {
        hw_id = hw_k2;
    }
    else {
        hw_id = hw_no;
    }

    return hw_id;
}

int hal_read_hw_id(void)
{
    return hw_id;
}

float hal_read_MCB12V_voltage(void)
{
    float v = 0.0f;
    float mcb_12v = 0.0f;
    for(int i = 0; i < 10; i++)
    {
        mcb_12v = read_voltage(MCB_V) * 4.9f;
        v += mcb_12v;
        hal_delay_1ms(1);
    }
    v /= 10.0f;
#ifdef ADC_DEBUG
    mid_log_print(LOG_VERBOSE, "MCB Voltage: %d.%02d\r\n", (int)v, (int)(v * 100) % 100);
#endif
    return v;
}

float hal_read_EXT12V_voltage(void)
{
    float v = 0.0f;
    float ext_12v = 0.0f;
    for(int i = 0; i < 10; i++)
    {
        ext_12v = read_voltage(EXT_V) * 4.9f;
        v += ext_12v;
        hal_delay_1ms(1);
    }
    v /= 10.0f;
#ifdef ADC_DEBUG
    mid_log_print(LOG_VERBOSE, "EXT Voltage: %d.%02d\r\n", (int)v, (int)(v * 100) % 100);
#endif
    return v;
}

float hal_read_MCB12V_current(void)
{
    float a = 0.0f;
    float mcb_12v_current = 0.0f;
    for(int i = 0; i < 10; i++)
    {
        mcb_12v_current = read_current(MCB_I);
        a += mcb_12v_current;
        hal_delay_1ms(1);
    }
    a /= 10.0f;
#ifdef ADC_DEBUG
    mid_log_print(LOG_VERBOSE, "MCB Current: %d.%02d\r\n", (int)a, (int)(a * 100) % 100);
#endif
    return a;
}

float hal_read_EXT12V_current(void)
{
    float a = 0.0f;
    float ext_12v_current = 0.0f;
    for(int i = 0; i < 10; i++)
    {
        ext_12v_current = read_current(EXT_I);
        a += ext_12v_current;
        hal_delay_1ms(1);
    }
    a /= 10.0f;
#ifdef ADC_DEBUG
    mid_log_print(LOG_VERBOSE, "EXT Current: %d.%02d\r\n", (int)a, (int)(a * 100) % 100);
#endif
    return a;
}

float mcb_current_buf[10] = {0};
float mcb_current_sum = 0;
uint8_t count = 0;
void hal_MCB12V_current_sample(void)
{
    float mcb_12v_current = read_current(MCB_I);

    mcb_current_sum -= mcb_current_buf[count];
    mcb_current_buf[count] = mcb_12v_current;
    mcb_current_sum += mcb_12v_current;

    count = (count + 1) % 10;
}

float hal_get_MCB12V_current_sample(void)
{
    return mcb_current_sum / 10.0f;
}

int hal_get_MCB12V_status(void) {
    return MCB12V_status = calculate_12v_status(read_voltage(MCB_V));
}

int hal_get_EXT12V_status(void) {   
    return EXT12V_status = calculate_12v_status(read_voltage(EXT_V));
}