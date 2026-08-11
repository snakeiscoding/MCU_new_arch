#ifndef HAL_SYSTICK_H__
#define HAL_SYSTICK_H__

#include <stdint.h>

void hal_systick_config(void);
void hal_delay_1ms(uint32_t count);
void hal_delay_decrement(void);

#endif //HAL_SYSTICK_H__

