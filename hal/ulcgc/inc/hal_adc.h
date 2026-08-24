#ifndef HAL_ADC_H__
#define HAL_ADC_H__

enum hardware_version {
    hw_k1,
    hw_k2,
    hw_k3,
    hw_no
};

enum power_status_enum {
    POWER_12V_OK,
    POWER_12V_LOW,
    POWER_12V_HIGH,
    POWER_12V_NO_POWER
};

int hal_get_hw_id(void);
int hal_read_hw_id(void);
int hal_get_MCB12V_status(void);
int hal_get_EXT12V_status(void);
float hal_read_MCB12V_voltage(void);
float hal_read_EXT12V_voltage(void);
float hal_read_MCB12V_current(void);
float hal_read_EXT12V_current(void);

void hal_MCB12V_current_sample(void);
float hal_get_MCB12V_current_sample(void);

void hal_adc_init(void);

#endif //HAL_ADC_H__