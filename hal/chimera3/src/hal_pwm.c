#include "hal_pwm.h"
#include "hal_stm32f4xx_it.h"
#include "stm32f4xx.h"

TIM_HandleTypeDef Tim4Handle;
TIM_OC_InitTypeDef	Tim4OutConfig;

#define  PERIOD_VALUE   ((uint16_t)666 - 1)
#define  PULSE4_VALUE   ((uint16_t)83)

#define GPIO_PWM_PIN_CHANNEL2   GPIO_PIN_13

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef   GPIO_InitStruct;
    if(htim->Instance == TIM4) {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* TIMx Peripheral clock enable */
        __HAL_RCC_TIM4_CLK_ENABLE();
        /* Enable GPIO Channels Clock */
        __HAL_RCC_GPIOD_CLK_ENABLE();
		
        /* Common configuration for all channels */
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
		
        GPIO_InitStruct.Pin = GPIO_PWM_PIN_CHANNEL2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

void pwm_pd13_init(void)
{
    uint32_t uhPrescalerValue = 0;
    /* Compute the prescaler value to have TIM3 counter clock equal to 21 MHz */
    uhPrescalerValue = (uint32_t) ((SystemCoreClock /2) / 21000000) - 1;
    /* Initialize TIMx peripheral as follow:
        + Prescaler = (SystemCoreClock/2)/21000000
        + Period = 665
        + ClockDivision = 0
        + Counter direction = Up
    */
    Tim4Handle.Instance = TIM4;
    
    Tim4Handle.Init.Prescaler = uhPrescalerValue;
    Tim4Handle.Init.Period = PERIOD_VALUE;
    Tim4Handle.Init.ClockDivision = 0;
    Tim4Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim4Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    /* Initialization Error */
    if(HAL_TIM_PWM_Init(&Tim4Handle) != HAL_OK)	{
        Error_Handler();
    }

    /*##-2- Configure the PWM channels #########################################*/ 
    /* Common configuration for all channels */
    Tim4OutConfig.OCMode = TIM_OCMODE_PWM1;
    Tim4OutConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    //  Tim4OutConfig.OCPolarity = TIM_OCPOLARITY_LOW;
    Tim4OutConfig.OCFastMode = TIM_OCFAST_DISABLE;
    
    /* Set the pulse value for channel 2 */
    Tim4OutConfig.Pulse = PULSE4_VALUE;
    if(HAL_TIM_PWM_ConfigChannel(&Tim4Handle, &Tim4OutConfig, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
}

void hal_pwm_set_enable(enum PWM_PIN_ID pin_id, bool enable)
{
    switch(pin_id) {
    case PWM_PD13:
        if(enable) {
            if(HAL_TIM_PWM_Start(&Tim4Handle, TIM_CHANNEL_2) != HAL_OK) {
                Error_Handler();
            }
        }
        else {
            if(HAL_TIM_PWM_Stop(&Tim4Handle, TIM_CHANNEL_2) != HAL_OK) {
                Error_Handler();
            }
        }
        break;
    }
}

void hal_pwm_set_pulse(enum PWM_PIN_ID pin_id, uint32_t pulse)
{
    switch(pin_id) {
    case PWM_PD13:
        if(HAL_TIM_PWM_Stop(&Tim4Handle, TIM_CHANNEL_2) != HAL_OK) {
            Error_Handler();
        }
        Tim4OutConfig.Pulse = (uint32_t)(665 * pulse / 100);
        if(HAL_TIM_PWM_ConfigChannel(&Tim4Handle, &Tim4OutConfig, TIM_CHANNEL_2) != HAL_OK) {
            Error_Handler();
        }
        break;
    }
}

void hal_pwm_init(void)
{
    pwm_pd13_init(); //FAN PWM
}

void hal_pwm_deinit(void)
{
}
