#include "hal_gpio.h"
#include "hal_systick.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t mode;
    uint32_t pull;
    uint32_t speed;
    void (*clk_enable)(void);
} output_gpio_config_t;


typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t mode;
    uint32_t pull;
    uint32_t speed;
    uint16_t exti;
    void (*clk_enable)(void);
} input_gpio_config_t;

static const input_gpio_config_t INPUT_GPIO_LIST[INPUT_GPIO_TOTAL_NUM] = {
    [FAN_RPM_GPIO] = {FAN_RPM_GPIO_PORT, FAN_RPM_GPIO_PIN, FAN_RPM_GPIO_MODE, FAN_RPM_GPIO_PULL, FAN_RPM_GPIO_SPEED, FAN_RPM_GPIO_EXTI, FAN_RPM_GPIO_CLK},
};

static const output_gpio_config_t OUTPUT_GPIO_LIST[OUTPUT_GPIO_TOTAL_NUM] = {
    [SYS_NRST_GPIO] = {SYS_NRST_GPIO_PORT, SYS_NRST_GPIO_PIN, SYS_NRST_GPIO_MODE, SYS_NRST_GPIO_PULL, SYS_NRST_GPIO_SPEED, SYS_NRST_GPIO_CLK},
    [ERP_EN_GPIO] = {ERP_EN_GPIO_PORT, ERP_EN_GPIO_PIN, ERP_EN_GPIO_MODE, ERP_EN_GPIO_PULL, ERP_EN_GPIO_SPEED, ERP_EN_GPIO_CLK},
    [EXT_EN_GPIO] = {EXT_EN_GPIO_PORT, EXT_EN_GPIO_PIN, EXT_EN_GPIO_MODE, EXT_EN_GPIO_PULL, EXT_EN_GPIO_SPEED, EXT_EN_GPIO_CLK},
    [ONOFF_GPIO] = {ONOFF_GPIO_PORT, ONOFF_GPIO_PIN, ONOFF_GPIO_MODE, ONOFF_GPIO_PULL, ONOFF_GPIO_SPEED, ONOFF_GPIO_CLK},
    [EXT12V_EN_GPIO] = {EXT12V_EN_GPIO_PORT, EXT12V_EN_GPIO_PIN, EXT12V_EN_GPIO_MODE, EXT12V_EN_GPIO_PULL, EXT12V_EN_GPIO_SPEED, EXT12V_EN_GPIO_CLK},
    [MCB_EN_GPIO] = {MCB_EN_GPIO_PORT, MCB_EN_GPIO_PIN, MCB_EN_GPIO_MODE, MCB_EN_GPIO_PULL, MCB_EN_GPIO_SPEED, MCB_EN_GPIO_CLK},
    [LCB_RS485_TX_GPIO] = {LCB_RS485_TX_GPIO_PORT, LCB_RS485_TX_GPIO_PIN, LCB_RS485_TX_GPIO_MODE, LCB_RS485_TX_GPIO_PULL, LCB_RS485_TX_GPIO_SPEED, LCB_RS485_TX_GPIO_CLK},
    [LCB_RS485_WRITE_GPIO] = {LCB_RS485_WRITE_GPIO_PORT, LCB_RS485_WRITE_GPIO_PIN, LCB_RS485_WRITE_GPIO_MODE, LCB_RS485_WRITE_GPIO_PULL, LCB_RS485_WRITE_GPIO_SPEED, LCB_RS485_WRITE_GPIO_CLK},
    [LCB_RS485M_EN_GPIO] = {LCB_RS485M_EN_GPIO_PORT, LCB_RS485M_EN_GPIO_PIN, LCB_RS485M_EN_GPIO_MODE, LCB_RS485M_EN_GPIO_PULL, LCB_RS485M_EN_GPIO_SPEED, LCB_RS485M_EN_GPIO_CLK},
    [LCB_RS485C_EN_GPIO] = {LCB_RS485C_EN_GPIO_PORT, LCB_RS485C_EN_GPIO_PIN, LCB_RS485C_EN_GPIO_MODE, LCB_RS485C_EN_GPIO_PULL, LCB_RS485C_EN_GPIO_SPEED, LCB_RS485C_EN_GPIO_CLK},
    [VOICE_NRST_GPIO] = {VOICE_NRST_GPIO_PORT, VOICE_NRST_GPIO_PIN, VOICE_NRST_GPIO_MODE, VOICE_NRST_GPIO_PULL, VOICE_NRST_GPIO_SPEED, VOICE_NRST_GPIO_CLK},
    [VOICE_S1_GPIO] = {VOICE_S1_GPIO_PORT, VOICE_S1_GPIO_PIN, VOICE_S1_GPIO_MODE, VOICE_S1_GPIO_PULL, VOICE_S1_GPIO_SPEED, VOICE_S1_GPIO_CLK},
    [VOICE_S2_GPIO] = {VOICE_S2_GPIO_PORT, VOICE_S2_GPIO_PIN, VOICE_S2_GPIO_MODE, VOICE_S2_GPIO_PULL, VOICE_S2_GPIO_SPEED, VOICE_S2_GPIO_CLK},
    [VOICE_S3_GPIO] = {VOICE_S3_GPIO_PORT, VOICE_S3_GPIO_PIN, VOICE_S3_GPIO_MODE, VOICE_S3_GPIO_PULL, VOICE_S3_GPIO_SPEED, VOICE_S3_GPIO_CLK},
    [VOICE_S4_GPIO] = {VOICE_S4_GPIO_PORT, VOICE_S4_GPIO_PIN, VOICE_S4_GPIO_MODE, VOICE_S4_GPIO_PULL, VOICE_S4_GPIO_SPEED, VOICE_S4_GPIO_CLK},
};

void hal_gpio_default_state_set()
{
    hal_gpio_set(SYS_NRST_GPIO, GPIO_HIGH);
    hal_gpio_set(ERP_EN_GPIO, GPIO_LOW);
    hal_gpio_set(EXT_EN_GPIO, GPIO_HIGH);
    hal_delay_1ms(100);
    hal_gpio_set(ONOFF_GPIO, GPIO_HIGH);
    hal_gpio_set(EXT12V_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCB_EN_GPIO, GPIO_LOW);
    hal_gpio_set(LCB_RS485_WRITE_GPIO, GPIO_LOW);
    hal_gpio_set(LCB_RS485M_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(LCB_RS485C_EN_GPIO, GPIO_LOW);
    hal_gpio_set(VOICE_NRST_GPIO, GPIO_LOW);
    hal_gpio_set(VOICE_S1_GPIO, GPIO_LOW);
    hal_gpio_set(VOICE_S2_GPIO, GPIO_LOW);
    hal_gpio_set(VOICE_S3_GPIO, GPIO_LOW);
    hal_gpio_set(VOICE_S4_GPIO, GPIO_LOW);
}

void hal_gpio_module_init()
{
    for(int i = 0; i < INPUT_GPIO_TOTAL_NUM; i++) {
        hal_input_gpio_init(i);
    }
    for(int i = 0; i < OUTPUT_GPIO_TOTAL_NUM; i++) {
        hal_output_gpio_init(i);
    }
    hal_gpio_default_state_set();
}

void hal_input_gpio_init(input_gpio_list_num gpio_num)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    HAL_GPIO_DeInit(INPUT_GPIO_LIST[gpio_num].port, INPUT_GPIO_LIST[gpio_num].pin);
    INPUT_GPIO_LIST[gpio_num].clk_enable();
    GPIO_InitStruct.Pin =  INPUT_GPIO_LIST[gpio_num].pin;
    GPIO_InitStruct.Mode = INPUT_GPIO_LIST[gpio_num].mode;
    GPIO_InitStruct.Pull = INPUT_GPIO_LIST[gpio_num].pull;
    GPIO_InitStruct.Speed = INPUT_GPIO_LIST[gpio_num].speed;
    HAL_GPIO_Init(INPUT_GPIO_LIST[gpio_num].port, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(INPUT_GPIO_LIST[gpio_num].exti, 2, 0);
    HAL_NVIC_EnableIRQ(INPUT_GPIO_LIST[gpio_num].exti);
}

void hal_output_gpio_init(output_gpio_list_num gpio_num)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    HAL_GPIO_DeInit(OUTPUT_GPIO_LIST[gpio_num].port, OUTPUT_GPIO_LIST[gpio_num].pin);
    OUTPUT_GPIO_LIST[gpio_num].clk_enable();    
    GPIO_InitStruct.Pin =  OUTPUT_GPIO_LIST[gpio_num].pin;
    GPIO_InitStruct.Mode = OUTPUT_GPIO_LIST[gpio_num].mode;
    GPIO_InitStruct.Pull = OUTPUT_GPIO_LIST[gpio_num].pull;
    GPIO_InitStruct.Speed = OUTPUT_GPIO_LIST[gpio_num].speed;
    HAL_GPIO_Init(OUTPUT_GPIO_LIST[gpio_num].port, &GPIO_InitStruct);
}

void hal_gpio_set(output_gpio_list_num gpio_num, gpio_status set_gpio_status)
{
    if(set_gpio_status == GPIO_LOW) {
        HAL_GPIO_WritePin(OUTPUT_GPIO_LIST[gpio_num].port, OUTPUT_GPIO_LIST[gpio_num].pin, GPIO_PIN_RESET);
    }
    else if(set_gpio_status == GPIO_HIGH) { 
        HAL_GPIO_WritePin(OUTPUT_GPIO_LIST[gpio_num].port, OUTPUT_GPIO_LIST[gpio_num].pin, GPIO_PIN_SET);
    }
}