#ifndef HAL_PWM_H__
#define HAL_PWM_H__

#include <stdbool.h>
#include <stdint.h>

enum PWM_PIN_ID {
    PWM_PA6,
};

void hal_pwm_set_pulse(enum PWM_PIN_ID pin_id, uint32_t pulse);
void hal_pwm_set_enable(enum PWM_PIN_ID pin_id, bool enable);
void hal_pwm_init(void);
void hal_pwm_deinit(void);

#endif //HAL_PWM_H__