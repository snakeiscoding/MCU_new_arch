#ifndef HAL_ADC_H__
#define HAL_ADC_H__

enum hardware_version {
    hw_k1,
    hw_k2,
    hw_k3,
    hw_kL,
    hw_no
};

void hal_adc_init();
int hal_get_hw_id(void);

#endif //HAL_ADC_H__