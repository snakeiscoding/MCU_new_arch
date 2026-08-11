#include "gd32f5xx.h"
#include "hal_systick.h"

volatile static uint32_t delay;

void hal_systick_config(void)
{
    if(SysTick_Config(SystemCoreClock / 1000U)) {
        /* capture error */
        while(1) {
        }
    }
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void hal_delay_1ms(uint32_t count)
{
    delay = count;
    while(0U != delay) {
    }
}

void hal_delay_decrement(void)
{
    if(0U != delay) {
        delay--;
    }
}
