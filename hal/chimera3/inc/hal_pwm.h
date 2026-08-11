#ifndef HAL_PWM_H__
#define HAL_PWM_H__

#include <stdbool.h>
#include <stdint.h>

enum PWM_PIN_ID {
    PWM_PD13,
};

void hal_pwm_set_enable(enum PWM_PIN_ID pin_id, bool enable);
void hal_pwm_set_pulse(enum PWM_PIN_ID pin_id, uint32_t pulse);
void hal_pwm_init(void);

#endif //HAL_I2C_H__