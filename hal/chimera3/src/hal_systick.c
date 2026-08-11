#include "hal_systick.h"
#include "mid_scheduler.h"
#include "stm32f4xx.h"

extern volatile uint64_t time_1ms;
volatile static uint32_t delay;

void hal_systick_config(void)
{
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    } 
            
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        while(1) {
        }
    }
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_IncTick(void)
{
    time_1ms++;
    hal_delay_decrement();
    mid_1ms_tick_handler();
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
