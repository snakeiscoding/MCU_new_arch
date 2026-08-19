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
    [KEYV_D1_GPIO] = {KEYV_D1_GPIO_PORT, KEYV_D1_GPIO_PIN, KEYV_D1_GPIO_MODE, KEYV_D1_GPIO_PULL, KEYV_D1_GPIO_SPEED, KEYV_D1_GPIO_EXTI, KEYV_D1_GPIO_CLK},
    [KEYV_D2_GPIO] = {KEYV_D2_GPIO_PORT, KEYV_D2_GPIO_PIN, KEYV_D2_GPIO_MODE, KEYV_D2_GPIO_PULL, KEYV_D2_GPIO_SPEED, KEYV_D2_GPIO_EXTI, KEYV_D2_GPIO_CLK},
    [SAFETY_KEY_GPIO] = {SAFETY_KEY_GPIO_PORT, SAFETY_KEY_GPIO_PIN, SAFETY_KEY_GPIO_MODE, SAFETY_KEY_GPIO_PULL, SAFETY_KEY_GPIO_SPEED, SAFETY_KEY_GPIO_EXTI, SAFETY_KEY_GPIO_CLK},
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
    [KEYH_D1_GPIO] = {KEYH_D1_GPIO_PORT, KEYH_D1_GPIO_PIN, KEYH_D1_GPIO_MODE, KEYH_D1_GPIO_PULL, KEYH_D1_GPIO_SPEED, KEYH_D1_GPIO_CLK},
    [KEYH_D2_GPIO] = {KEYH_D2_GPIO_PORT, KEYH_D2_GPIO_PIN, KEYH_D2_GPIO_MODE, KEYH_D2_GPIO_PULL, KEYH_D2_GPIO_SPEED, KEYH_D2_GPIO_CLK},
    [KEYH_D3_GPIO] = {KEYH_D3_GPIO_PORT, KEYH_D3_GPIO_PIN, KEYH_D3_GPIO_MODE, KEYH_D3_GPIO_PULL, KEYH_D3_GPIO_SPEED, KEYH_D3_GPIO_CLK},
    [KEYH_D4_GPIO] = {KEYH_D4_GPIO_PORT, KEYH_D4_GPIO_PIN, KEYH_D4_GPIO_MODE, KEYH_D4_GPIO_PULL, KEYH_D4_GPIO_SPEED, KEYH_D4_GPIO_CLK},
    [KEYH_D5_GPIO] = {KEYH_D5_GPIO_PORT, KEYH_D5_GPIO_PIN, KEYH_D5_GPIO_MODE, KEYH_D5_GPIO_PULL, KEYH_D5_GPIO_SPEED, KEYH_D5_GPIO_CLK},
    [KEYH_D6_GPIO] = {KEYH_D6_GPIO_PORT, KEYH_D6_GPIO_PIN, KEYH_D6_GPIO_MODE, KEYH_D6_GPIO_PULL, KEYH_D6_GPIO_SPEED, KEYH_D6_GPIO_CLK},
    [CSAFE_PWR_EN_GPIO] = {CSAFE_PWR_EN_GPIO_PORT, CSAFE_PWR_EN_GPIO_PIN, CSAFE_PWR_EN_GPIO_MODE, CSAFE_PWR_EN_GPIO_PULL, CSAFE_PWR_EN_GPIO_SPEED, CSAFE_PWR_EN_GPIO_CLK},
    [RFID_5V_EN_GPIO] = {RFID_5V_EN_GPIO_PORT, RFID_5V_EN_GPIO_PIN, RFID_5V_EN_GPIO_MODE, RFID_5V_EN_GPIO_PULL, RFID_5V_EN_GPIO_SPEED, RFID_5V_EN_GPIO_CLK},
    [TV_12V_EN_GPIO] = {TV_12V_EN_GPIO_PORT, TV_12V_EN_GPIO_PIN, TV_12V_EN_GPIO_MODE, TV_12V_EN_GPIO_PULL, TV_12V_EN_GPIO_SPEED, TV_12V_EN_GPIO_CLK},
    [HR_5V_EN_GPIO] = {HR_5V_EN_GPIO_PORT, HR_5V_EN_GPIO_PIN, HR_5V_EN_GPIO_MODE, HR_5V_EN_GPIO_PULL, HR_5V_EN_GPIO_SPEED, HR_5V_EN_GPIO_CLK},
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
    /* Keypad drive lines idle high; the scan pulls one low at a time. */
    hal_gpio_set(KEYH_D1_GPIO, GPIO_HIGH);
    hal_gpio_set(KEYH_D2_GPIO, GPIO_HIGH);
    hal_gpio_set(KEYH_D3_GPIO, GPIO_HIGH);
    hal_gpio_set(KEYH_D4_GPIO, GPIO_HIGH);
    hal_gpio_set(KEYH_D5_GPIO, GPIO_HIGH);
    hal_gpio_set(KEYH_D6_GPIO, GPIO_HIGH);
    /* Single-pin power rails. The legacy PwrFlowCtrl[] brings all four up at
     * boot (GPIO_PIN_SET), so the SoC finds csafe, RFID, tuner 12V and heart
     * rate powered unless it asks for them off. */
    hal_gpio_set(CSAFE_PWR_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(RFID_5V_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(TV_12V_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(HR_5V_EN_GPIO, GPIO_HIGH);
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
    /* Polled inputs carry GPIO_EXTI_NONE and must not have an interrupt armed. */
    if(INPUT_GPIO_LIST[gpio_num].exti != GPIO_EXTI_NONE) {
        HAL_NVIC_SetPriority(INPUT_GPIO_LIST[gpio_num].exti, 2, 0);
        HAL_NVIC_EnableIRQ(INPUT_GPIO_LIST[gpio_num].exti);
    }
}

gpio_status hal_gpio_get(input_gpio_list_num gpio_num)
{
    if(HAL_GPIO_ReadPin(INPUT_GPIO_LIST[gpio_num].port, INPUT_GPIO_LIST[gpio_num].pin) == GPIO_PIN_RESET) {
        return GPIO_LOW;
    }
    return GPIO_HIGH;
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