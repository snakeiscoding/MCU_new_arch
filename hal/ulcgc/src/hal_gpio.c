#include <stdio.h>
#include "gd32f5xx.h"
#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd.h"
#include "hal_adc.h"
#include "hal_gpio.h"

typedef struct {
    uint32_t port;
    uint32_t pin;
    rcu_periph_enum clk;
    uint32_t exti_line;
    uint8_t exti_port;
    uint8_t exti_pin;
    uint8_t exti_irq;
} input_gpio_config_t;

typedef struct {
    uint32_t port;
    uint32_t pin;
    uint32_t mode;
    uint32_t pull;
    uint32_t speed;
    rcu_periph_enum clk;
} output_gpio_config_t;

static const input_gpio_config_t INPUT_GPIO_LIST[INPUT_GPIO_TOTAL_NUM] = {
    [MCU_SW_POSITION_GPIO] = {MCU_SW_POSITION_GPIO_PORT, MCU_SW_POSITION_GPIO_PIN, MCU_SW_POSITION_GPIO_CLK, MCU_SW_POSITION_EXTI_LINE, MCU_SW_POSITION_EXTI_PORT, MCU_SW_POSITION_EXTI_PIN, MCU_SW_POSITION_EXTI_IRQn},
    [SOC_RESET_GPIO] = {SOC_RESET_GPIO_PORT, SOC_RESET_GPIO_PIN, SOC_RESET_GPIO_CLK, SOC_RESET_EXTI_LINE, SOC_RESET_EXTI_PORT, SOC_RESET_EXTI_PIN, SOC_RESET_EXTI_IRQn},
    [MCU_RECOVERY_GPIO] = {MCU_RECOVERY_GPIO_PORT, MCU_RECOVERY_GPIO_PIN, MCU_RECOVERY_GPIO_CLK, MCU_RECOVERY_EXTI_LINE, MCU_RECOVERY_EXTI_PORT, MCU_RECOVERY_EXTI_PIN, MCU_RECOVERY_EXTI_IRQn},
    [ERP_IR_INT_GPIO] = {ERP_IR_INT_GPIO_PORT, ERP_IR_INT_GPIO_PIN, ERP_IR_INT_GPIO_CLK, ERP_IR_INT_EXTI_LINE, ERP_IR_INT_EXTI_PORT, ERP_IR_INT_EXTI_PIN, ERP_IR_INT_EXTI_IRQn},
    [MCU_KEY_INT_GPIO] = {MCU_KEY_INT_GPIO_PORT, MCU_KEY_INT_GPIO_PIN, MCU_KEY_INT_GPIO_CLK, MCU_KEY_INT_EXTI_LINE, MCU_KEY_INT_EXTI_PORT, MCU_KEY_INT_EXTI_PIN, MCU_KEY_INT_EXTI_IRQn},
};

static const output_gpio_config_t OUTPUT_GPIO_LIST[OUTPUT_GPIO_TOTAL_NUM] = {
    [MCU_EUPS_GPIO] = {MCU_EUPS_GPIO_PORT, MCU_EUPS_GPIO_PIN, MCU_EUPS_GPIO_MODE, MCU_EUPS_GPIO_PULL, MCU_EUPS_GPIO_SPEED, MCU_EUPS_GPIO_CLK},
    [SOC_5VS5_PEN_GPIO] = {SOC_5VS5_PEN_GPIO_PORT, SOC_5VS5_PEN_GPIO_PIN, SOC_5VS5_PEN_GPIO_MODE, SOC_5VS5_PEN_GPIO_PULL, SOC_5VS5_PEN_GPIO_SPEED, SOC_5VS5_PEN_GPIO_CLK},
    [MCU_POWERON_KEY_GPIO] = {MCU_POWERON_KEY_GPIO_PORT, MCU_POWERON_KEY_GPIO_PIN, MCU_POWERON_KEY_GPIO_MODE, MCU_POWERON_KEY_GPIO_PULL, MCU_POWERON_KEY_GPIO_SPEED, MCU_POWERON_KEY_GPIO_CLK},
    [MCU_RESET_KEY_GPIO] = {MCU_RESET_KEY_GPIO_PORT, MCU_RESET_KEY_GPIO_PIN, MCU_RESET_KEY_GPIO_MODE, MCU_RESET_KEY_GPIO_PULL, MCU_RESET_KEY_GPIO_SPEED, MCU_RESET_KEY_GPIO_CLK},
    [MCU_ERP_PEN_GPIO] = {MCU_ERP_PEN_GPIO_PORT, MCU_ERP_PEN_GPIO_PIN, MCU_ERP_PEN_GPIO_MODE, MCU_ERP_PEN_GPIO_PULL, MCU_ERP_PEN_GPIO_SPEED, MCU_ERP_PEN_GPIO_CLK},
    [MCU_ERP_LED_GPIO] = {MCU_ERP_LED_GPIO_PORT, MCU_ERP_LED_GPIO_PIN, MCU_ERP_LED_GPIO_MODE, MCU_ERP_LED_GPIO_PULL, MCU_ERP_LED_GPIO_SPEED, MCU_ERP_LED_GPIO_CLK},
    [ERP_WHITE_GPIO] = {ERP_WHITE_GPIO_PORT, ERP_WHITE_GPIO_PIN, ERP_WHITE_GPIO_MODE, ERP_WHITE_GPIO_PULL, ERP_WHITE_GPIO_SPEED, ERP_WHITE_GPIO_CLK},
    [ERP_GREEN_GPIO] = {ERP_GREEN_GPIO_PORT, ERP_GREEN_GPIO_PIN, ERP_GREEN_GPIO_MODE, ERP_GREEN_GPIO_PULL, ERP_GREEN_GPIO_SPEED, ERP_GREEN_GPIO_CLK},
    [QI_9VPEN_GPIO] = {QI_9VPEN_GPIO_PORT, QI_9VPEN_GPIO_PIN, QI_9VPEN_GPIO_MODE, QI_9VPEN_GPIO_PULL, QI_9VPEN_GPIO_SPEED, QI_9VPEN_GPIO_CLK},
    [QI_12VPEN_GPIO] = {QI_12VPEN_GPIO_PORT, QI_12VPEN_GPIO_PIN, QI_12VPEN_GPIO_MODE, QI_12VPEN_GPIO_PULL, QI_12VPEN_GPIO_SPEED, QI_12VPEN_GPIO_CLK},
    [MCU_UART3_TXD_GPIO] = {MCU_UART3_TXD_GPIO_PORT, MCU_UART3_TXD_GPIO_PIN, MCU_UART3_TXD_GPIO_MODE, MCU_UART3_TXD_GPIO_PULL, MCU_UART3_TXD_GPIO_SPEED, MCU_UART3_TXD_GPIO_CLK},
    [MCU_UART3_RTS_GPIO] = {MCU_UART3_RTS_GPIO_PORT, MCU_UART3_RTS_GPIO_PIN, MCU_UART3_RTS_GPIO_MODE, MCU_UART3_RTS_GPIO_PULL, MCU_UART3_RTS_GPIO_SPEED, MCU_UART3_RTS_GPIO_CLK},
    [MCU_SOC485_TXEN_GPIO] = {MCU_SOC485_TXEN_GPIO_PORT, MCU_SOC485_TXEN_GPIO_PIN, MCU_SOC485_TXEN_GPIO_MODE, MCU_SOC485_TXEN_GPIO_PULL, MCU_SOC485_TXEN_GPIO_SPEED, MCU_SOC485_TXEN_GPIO_CLK},
    [CAN_12V_EN_GPIO] = {CAN_12V_EN_GPIO_PORT, CAN_12V_EN_GPIO_PIN, CAN_12V_EN_GPIO_MODE, CAN_12V_EN_GPIO_PULL, CAN_12V_EN_GPIO_SPEED, CAN_12V_EN_GPIO_CLK},
    [CAN_WAKE_UP_GPIO] = {CAN_WAKE_UP_GPIO_PORT, CAN_WAKE_UP_GPIO_PIN, CAN_WAKE_UP_GPIO_MODE, CAN_WAKE_UP_GPIO_PULL, CAN_WAKE_UP_GPIO_SPEED, CAN_WAKE_UP_GPIO_CLK},
    [CAN_MODE_GPIO] = {CAN_MODE_GPIO_PORT, CAN_MODE_GPIO_PIN, CAN_MODE_GPIO_MODE, CAN_MODE_GPIO_PULL, CAN_MODE_GPIO_SPEED, CAN_MODE_GPIO_CLK},
    [MCU_CSAFE_PEN_GPIO] = {MCU_CSAFE_PEN_GPIO_PORT, MCU_CSAFE_PEN_GPIO_PIN, MCU_CSAFE_PEN_GPIO_MODE, MCU_CSAFE_PEN_GPIO_PULL, MCU_CSAFE_PEN_GPIO_SPEED, MCU_CSAFE_PEN_GPIO_CLK},
    [MCU_KEY_REST_GPIO] = {MCU_KEY_REST_GPIO_PORT, MCU_KEY_REST_GPIO_PIN, MCU_KEY_REST_GPIO_MODE, MCU_KEY_REST_GPIO_PULL, MCU_KEY_REST_GPIO_SPEED, MCU_KEY_REST_GPIO_CLK},
    [MCU_VOICE_PEN_GPIO] = {MCU_VOICE_PEN_GPIO_PORT, MCU_VOICE_PEN_GPIO_PIN, MCU_VOICE_PEN_GPIO_MODE, MCU_VOICE_PEN_GPIO_PULL, MCU_VOICE_PEN_GPIO_SPEED, MCU_VOICE_PEN_GPIO_CLK},
    [MCU_BEEP_GPIO] = {MCU_BEEP_GPIO_PORT, MCU_BEEP_GPIO_PIN, MCU_BEEP_GPIO_MODE, MCU_BEEP_GPIO_PULL, MCU_BEEP_GPIO_SPEED, MCU_BEEP_GPIO_CLK},
    [EXIT_PW_EN_GPIO] = {EXIT_PW_EN_GPIO_PORT, EXIT_PW_EN_GPIO_PIN, EXIT_PW_EN_GPIO_MODE, EXIT_PW_EN_GPIO_PULL, EXIT_PW_EN_GPIO_SPEED, EXIT_PW_EN_GPIO_CLK},
    [EXT_12V6_EN_GPIO] = {EXT_12V6_EN_GPIO_PORT, EXT_12V6_EN_GPIO_PIN, EXT_12V6_EN_GPIO_MODE, EXT_12V6_EN_GPIO_PULL, EXT_12V6_EN_GPIO_SPEED, EXT_12V6_EN_GPIO_CLK},
    [TP_PWR_EN_GPIO] = {TP_PWR_EN_GPIO_PORT, TP_PWR_EN_GPIO_PIN, TP_PWR_EN_GPIO_MODE, TP_PWR_EN_GPIO_PULL, TP_PWR_EN_GPIO_SPEED, TP_PWR_EN_GPIO_CLK},
    [M2_PEN_GPIO] = {M2_PEN_GPIO_PORT, M2_PEN_GPIO_PIN, M2_PEN_GPIO_MODE, M2_PEN_GPIO_PULL, M2_PEN_GPIO_SPEED, M2_PEN_GPIO_CLK},
    [MCU_HR_PEN_GPIO] = {MCU_HR_PEN_GPIO_PORT, MCU_HR_PEN_GPIO_PIN, MCU_HR_PEN_GPIO_MODE, MCU_HR_PEN_GPIO_PULL, MCU_HR_PEN_GPIO_SPEED, MCU_HR_PEN_GPIO_CLK},
    [MCU_RFID_PEN_GPIO] = {MCU_RFID_PEN_GPIO_PORT, MCU_RFID_PEN_GPIO_PIN, MCU_RFID_PEN_GPIO_MODE, MCU_RFID_PEN_GPIO_PULL, MCU_RFID_PEN_GPIO_SPEED, MCU_RFID_PEN_GPIO_CLK},
    [MCU_USB30_PWREN_GPIO] = {MCU_USB30_PWREN_GPIO_PORT, MCU_USB30_PWREN_GPIO_PIN, MCU_USB30_PWREN_GPIO_MODE, MCU_USB30_PWREN_GPIO_PULL, MCU_USB30_PWREN_GPIO_SPEED, MCU_USB30_PWREN_GPIO_CLK},
    [SDMMC0_PWREN_H_GPIO] = {SDMMC0_PWREN_H_GPIO_PORT, SDMMC0_PWREN_H_GPIO_PIN, SDMMC0_PWREN_H_GPIO_MODE, SDMMC0_PWREN_H_GPIO_PULL, SDMMC0_PWREN_H_GPIO_SPEED, SDMMC0_PWREN_H_GPIO_CLK},
    [RS232_5V_EN_GPIO] = {RS232_5V_EN_GPIO_PORT, RS232_5V_EN_GPIO_PIN, RS232_5V_EN_GPIO_MODE, RS232_5V_EN_GPIO_PULL, RS232_5V_EN_GPIO_SPEED, RS232_5V_EN_GPIO_CLK},
    [MCU_LED0_GPIO] = {MCU_LED0_GPIO_PORT, MCU_LED0_GPIO_PIN, MCU_LED0_GPIO_MODE, MCU_LED0_GPIO_PULL, MCU_LED0_GPIO_SPEED, MCU_LED0_GPIO_CLK},
};

void hal_output_gpio_default_state_set(void)
{
    hal_gpio_set(MCU_EUPS_GPIO, GPIO_HIGH);
    hal_gpio_set(EXIT_PW_EN_GPIO, GPIO_HIGH);

    hal_gpio_set(MCU_POWERON_KEY_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_RESET_KEY_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_ERP_PEN_GPIO, GPIO_HIGH);
    hal_gpio_set(ERP_WHITE_GPIO, GPIO_HIGH);

    if(hal_read_hw_id() == hw_k2) {
        hal_gpio_set(MCU_SOC485_TXEN_GPIO, GPIO_HIGH);
    }

    hal_gpio_set(EXT_12V6_EN_GPIO, GPIO_LOW);
    hal_gpio_set(SOC_5VS5_PEN_GPIO, GPIO_HIGH);

    hal_gpio_set(MCU_VOICE_PEN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCU_BEEP_GPIO, GPIO_LOW);

    hal_gpio_set(SDMMC0_PWREN_H_GPIO, GPIO_HIGH);
    hal_gpio_set(TP_PWR_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(RS232_5V_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCU_USB30_PWREN_GPIO, GPIO_HIGH);
    hal_gpio_set(M2_PEN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCU_RFID_PEN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCU_HR_PEN_GPIO, GPIO_HIGH);

    hal_gpio_set(QI_9VPEN_GPIO, GPIO_HIGH);
    hal_gpio_set(QI_12VPEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_CSAFE_PEN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCU_LED0_GPIO, GPIO_LOW);

    hal_gpio_set(CAN_12V_EN_GPIO, GPIO_HIGH);
    hal_gpio_set(CAN_MODE_GPIO, GPIO_LOW);

    app_soc_cmd_control_power_state_set(CSAFE, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(RFID, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(HEART_RATE, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(GYMKIT, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(USB_5V, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(EXT_12V, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(IR_5V, GPIO_HIGH);
    app_soc_cmd_control_power_state_set(RS232_5V, GPIO_HIGH);
}

void hal_gpio_module_init(void)
{
    for(int i = 0; i < INPUT_GPIO_TOTAL_NUM; i++) {
        if(!(i == ERP_IR_INT_GPIO || i == MCU_KEY_INT_GPIO)) {
            hal_input_gpio_init(i);
        }
    }

    for(int i = 0; i < OUTPUT_GPIO_TOTAL_NUM; i++) {
        hal_output_gpio_init(i);
    }
    hal_output_gpio_default_state_set();
}

void hal_gpio_module_deinit(void)
{
    hal_gpio_set(MCU_LED0_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_VOICE_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_CSAFE_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_HR_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_RFID_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(M2_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_USB30_PWREN_GPIO, GPIO_LOW);
    hal_gpio_set(RS232_5V_EN_GPIO, GPIO_LOW);
    hal_gpio_set(TP_PWR_EN_GPIO, GPIO_LOW);
    hal_gpio_set(SDMMC0_PWREN_H_GPIO, GPIO_LOW);

    hal_gpio_set(QI_9VPEN_GPIO, GPIO_LOW);
    hal_gpio_set(QI_12VPEN_GPIO, GPIO_LOW);
    hal_gpio_set(CAN_12V_EN_GPIO, GPIO_LOW);

    hal_gpio_set(MCU_ERP_PEN_GPIO, GPIO_HIGH);
    hal_gpio_set(MCU_ERP_LED_GPIO, GPIO_HIGH);
    hal_gpio_set(ERP_GREEN_GPIO, GPIO_HIGH);
    hal_gpio_set(ERP_WHITE_GPIO, GPIO_LOW);
    hal_input_gpio_init(ERP_IR_INT_GPIO);

    if(hal_read_hw_id() == hw_k2) {
        hal_gpio_set(MCU_SOC485_TXEN_GPIO, GPIO_LOW);
    }

    hal_gpio_set(EXIT_PW_EN_GPIO, GPIO_LOW);
    hal_gpio_set(EXT_12V6_EN_GPIO, GPIO_LOW);

    hal_gpio_set(SOC_5VS5_PEN_GPIO, GPIO_LOW);
    hal_gpio_set(MCU_EUPS_GPIO, GPIO_LOW);
}

void hal_input_gpio_init(input_gpio_list_num gpio_num)
{
    nvic_irq_disable(INPUT_GPIO_LIST[gpio_num].exti_irq);
    exti_interrupt_flag_clear(INPUT_GPIO_LIST[gpio_num].exti_line);
    exti_interrupt_disable(INPUT_GPIO_LIST[gpio_num].exti_line);

    rcu_periph_clock_enable(INPUT_GPIO_LIST[gpio_num].clk);
    rcu_periph_clock_enable(RCU_SYSCFG);

    gpio_mode_set(INPUT_GPIO_LIST[gpio_num].port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, INPUT_GPIO_LIST[gpio_num].pin);
    syscfg_exti_line_config(INPUT_GPIO_LIST[gpio_num].exti_port, INPUT_GPIO_LIST[gpio_num].exti_pin);
    exti_init(INPUT_GPIO_LIST[gpio_num].exti_line, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(INPUT_GPIO_LIST[gpio_num].exti_line);
    nvic_irq_enable(INPUT_GPIO_LIST[gpio_num].exti_irq, 2U, 0U);
}

void hal_input_gpio_deinit(input_gpio_list_num gpio_num)
{
    nvic_irq_disable(INPUT_GPIO_LIST[gpio_num].exti_irq);
    exti_interrupt_flag_clear(INPUT_GPIO_LIST[gpio_num].exti_line);
    exti_interrupt_disable(INPUT_GPIO_LIST[gpio_num].exti_line);
}

void hal_output_gpio_init(output_gpio_list_num gpio_num)
{
    rcu_periph_clock_enable(OUTPUT_GPIO_LIST[gpio_num].clk);
    if(OUTPUT_GPIO_LIST[gpio_num].mode == GPIO_OUTPUT_PP) {
        gpio_mode_set(OUTPUT_GPIO_LIST[gpio_num].port, GPIO_MODE_OUTPUT, OUTPUT_GPIO_LIST[gpio_num].pull, OUTPUT_GPIO_LIST[gpio_num].pin);
        gpio_output_options_set(OUTPUT_GPIO_LIST[gpio_num].port, GPIO_OTYPE_PP, OUTPUT_GPIO_LIST[gpio_num].speed, OUTPUT_GPIO_LIST[gpio_num].pin);
        GPIO_BC(OUTPUT_GPIO_LIST[gpio_num].port) = OUTPUT_GPIO_LIST[gpio_num].pin;
    }
    else if(OUTPUT_GPIO_LIST[gpio_num].mode == GPIO_OUTPUT_OD) {
        gpio_mode_set(OUTPUT_GPIO_LIST[gpio_num].port, GPIO_MODE_OUTPUT, OUTPUT_GPIO_LIST[gpio_num].pull, OUTPUT_GPIO_LIST[gpio_num].pin);
        gpio_output_options_set(OUTPUT_GPIO_LIST[gpio_num].port, GPIO_OTYPE_OD, OUTPUT_GPIO_LIST[gpio_num].speed, OUTPUT_GPIO_LIST[gpio_num].pin);
        GPIO_BC(OUTPUT_GPIO_LIST[gpio_num].port) = OUTPUT_GPIO_LIST[gpio_num].pin;
    }
}

void hal_gpio_set(output_gpio_list_num gpio_num, gpio_status set_gpio_status)
{
    if(set_gpio_status == GPIO_LOW) {
        GPIO_BC(OUTPUT_GPIO_LIST[gpio_num].port) = OUTPUT_GPIO_LIST[gpio_num].pin;
    }
    else if(set_gpio_status == GPIO_HIGH) { 
        GPIO_BOP(OUTPUT_GPIO_LIST[gpio_num].port) = OUTPUT_GPIO_LIST[gpio_num].pin;
    }
}

uint8_t hal_gpio_get(input_gpio_list_num gpio_num)
{
    return gpio_input_bit_get(INPUT_GPIO_LIST[gpio_num].port, INPUT_GPIO_LIST[gpio_num].pin);
}