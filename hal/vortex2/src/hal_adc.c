#include <math.h>
#include "gd32f5xx_adc.h"
#include "hal_adc.h"
#include "hal_systick.h"

enum hardware_version hw_id = hw_k2;

uint16_t hal_adc_from_hw_voltage(void)
{
    /* ADC routine channel config */
    adc_routine_channel_config(ADC0, 0U, ADC_CHANNEL_13, ADC_SAMPLETIME_15);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    /* wait the end of conversion flag */
    while (!adc_flag_get(ADC0, ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    return adc_routine_data_read(ADC0);
}

float hal_read_hw_voltage_adc(void)
{
    uint16_t adc_value = hal_adc_from_hw_voltage();
    float voltage_data = (float)adc_value / 4096.0f * 3.3f;
    voltage_data = roundf(voltage_data * 1000) / 1000;
    return voltage_data;
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

    //printf("%d...\r\n",adc_channel_sample(ADC_CHANNEL_13));
    //log_print(LOG_VERBOSE,"HW ID:=%0.2f\r\n",hal_read_hw_voltage_adc());
    if((hal_read_hw_voltage_adc() >= 1.60)&&(hal_read_hw_voltage_adc() <= 1.70)) {
        hw_id = hw_k3;
        // log_print(LOG_VERBOSE,"Found JHT K3\r\n");
    }
    else if((hal_read_hw_voltage_adc() >= 0)&&(hal_read_hw_voltage_adc() <= 0.1)) {
        hw_id = hw_k2;
        // log_print(LOG_VERBOSE,"Found JHT K2\r\n");
    }
    else if((hal_read_hw_voltage_adc() >= 3.25)&&(hal_read_hw_voltage_adc() <= 3.3)) {
        hw_id = hw_k1;
        // log_print(LOG_VERBOSE,"Found JHT K1\r\n");
    }
    else if((hal_read_hw_voltage_adc() >= 2.10)&&(hal_read_hw_voltage_adc() <= 2.20)) {
        hw_id = hw_kL;
        // log_print(LOG_VERBOSE,"Found JHT K1\r\n");
    }
    else {
        hw_id = hw_no;
    }
}

int hal_get_hw_id(void)
{
    return hw_id;
}

void hal_adc_init(void)
{
    hal_hw_voltage_adc_init();
}