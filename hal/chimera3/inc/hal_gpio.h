#ifndef HAL_GPIO_H__
#define HAL_GPIO_H__

#include "stm32f4xx.h"

typedef enum {
    FAN_RPM_GPIO = 0,
    INPUT_GPIO_TOTAL_NUM,
} input_gpio_list_num;

typedef enum {
    SYS_NRST_GPIO = 0,
    ERP_EN_GPIO,
    EXT_EN_GPIO,
    ONOFF_GPIO,
    EXT12V_EN_GPIO,
    MCB_EN_GPIO,
    LCB_RS485_TX_GPIO,
    LCB_RS485_WRITE_GPIO,
    LCB_RS485M_EN_GPIO,
    LCB_RS485C_EN_GPIO,
    VOICE_NRST_GPIO,
    VOICE_S1_GPIO,
    VOICE_S2_GPIO,
    VOICE_S3_GPIO,
    VOICE_S4_GPIO,
    OUTPUT_GPIO_TOTAL_NUM,
} output_gpio_list_num;

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpio_status;

static inline void gpio_a_clk_en(void) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
static inline void gpio_b_clk_en(void) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
static inline void gpio_c_clk_en(void) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
static inline void gpio_d_clk_en(void) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
static inline void gpio_e_clk_en(void) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
static inline void gpio_f_clk_en(void) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
static inline void gpio_g_clk_en(void) { __HAL_RCC_GPIOG_CLK_ENABLE(); }
static inline void gpio_h_clk_en(void) { __HAL_RCC_GPIOH_CLK_ENABLE(); }
static inline void gpio_i_clk_en(void) { __HAL_RCC_GPIOI_CLK_ENABLE(); }

//GPIO_A OUTPUT
#define LCB_RS485_TX_GPIO_PIN       GPIO_PIN_2
#define LCB_RS485_TX_GPIO_PORT      GPIOA
#define LCB_RS485_TX_GPIO_MODE      GPIO_MODE_OUTPUT_PP
#define LCB_RS485_TX_GPIO_PULL      GPIO_NOPULL
#define LCB_RS485_TX_GPIO_SPEED     GPIO_SPEED_FREQ_VERY_HIGH
#define LCB_RS485_TX_GPIO_CLK       gpio_a_clk_en

#define LCB_RS485C_EN_GPIO_PIN      GPIO_PIN_7
#define LCB_RS485C_EN_GPIO_PORT     GPIOA
#define LCB_RS485C_EN_GPIO_MODE     GPIO_MODE_OUTPUT_PP
#define LCB_RS485C_EN_GPIO_PULL     GPIO_NOPULL
#define LCB_RS485C_EN_GPIO_SPEED    GPIO_SPEED_FREQ_VERY_HIGH
#define LCB_RS485C_EN_GPIO_CLK      gpio_a_clk_en

#define LCB_RS485_WRITE_GPIO_PIN    GPIO_PIN_15
#define LCB_RS485_WRITE_GPIO_PORT   GPIOA
#define LCB_RS485_WRITE_GPIO_MODE   GPIO_MODE_OUTPUT_PP
#define LCB_RS485_WRITE_GPIO_PULL   GPIO_NOPULL
#define LCB_RS485_WRITE_GPIO_SPEED  GPIO_SPEED_FREQ_VERY_HIGH
#define LCB_RS485_WRITE_GPIO_CLK    gpio_a_clk_en

/* AP890XX buzzer. The legacy Buzzer_AP890XX.c only ever drove nRST and S4 low
 * and never drove either high again, so both are simply parked low here. The
 * tone itself is a pulse on one of S1/S2/S3. */
#define VOICE_NRST_GPIO_PIN         GPIO_PIN_12
#define VOICE_NRST_GPIO_PORT        GPIOA
#define VOICE_NRST_GPIO_MODE        GPIO_MODE_OUTPUT_PP
#define VOICE_NRST_GPIO_PULL        GPIO_NOPULL
#define VOICE_NRST_GPIO_SPEED       GPIO_SPEED_FREQ_VERY_HIGH
#define VOICE_NRST_GPIO_CLK         gpio_a_clk_en

//GPIO_E OUTPUT
#define VOICE_S1_GPIO_PIN           GPIO_PIN_0
#define VOICE_S1_GPIO_PORT          GPIOE
#define VOICE_S1_GPIO_MODE          GPIO_MODE_OUTPUT_PP
#define VOICE_S1_GPIO_PULL          GPIO_NOPULL
#define VOICE_S1_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define VOICE_S1_GPIO_CLK           gpio_e_clk_en

#define VOICE_S2_GPIO_PIN           GPIO_PIN_1
#define VOICE_S2_GPIO_PORT          GPIOE
#define VOICE_S2_GPIO_MODE          GPIO_MODE_OUTPUT_PP
#define VOICE_S2_GPIO_PULL          GPIO_NOPULL
#define VOICE_S2_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define VOICE_S2_GPIO_CLK           gpio_e_clk_en

#define VOICE_S3_GPIO_PIN           GPIO_PIN_2
#define VOICE_S3_GPIO_PORT          GPIOE
#define VOICE_S3_GPIO_MODE          GPIO_MODE_OUTPUT_PP
#define VOICE_S3_GPIO_PULL          GPIO_NOPULL
#define VOICE_S3_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define VOICE_S3_GPIO_CLK           gpio_e_clk_en

#define VOICE_S4_GPIO_PIN           GPIO_PIN_3
#define VOICE_S4_GPIO_PORT          GPIOE
#define VOICE_S4_GPIO_MODE          GPIO_MODE_OUTPUT_PP
#define VOICE_S4_GPIO_PULL          GPIO_NOPULL
#define VOICE_S4_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define VOICE_S4_GPIO_CLK           gpio_e_clk_en

//GPIO_C INPUT
#define FAN_RPM_GPIO_PIN            GPIO_PIN_8
#define FAN_RPM_GPIO_PORT           GPIOC
#define FAN_RPM_GPIO_MODE           GPIO_MODE_IT_RISING_FALLING
#define FAN_RPM_GPIO_PULL           GPIO_PULLUP
#define FAN_RPM_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define FAN_RPM_GPIO_EXTI           EXTI9_5_IRQn
#define FAN_RPM_GPIO_CLK            gpio_c_clk_en

//GPIO_D OUTPUT
#define ERP_EN_GPIO_PIN             GPIO_PIN_11
#define ERP_EN_GPIO_PORT            GPIOD
#define ERP_EN_GPIO_MODE            GPIO_MODE_OUTPUT_PP
#define ERP_EN_GPIO_PULL            GPIO_NOPULL
#define ERP_EN_GPIO_SPEED           GPIO_SPEED_FREQ_VERY_HIGH
#define ERP_EN_GPIO_CLK             gpio_d_clk_en

//GPIO_F OUTPUT
#define ONOFF_GPIO_PIN              GPIO_PIN_4
#define ONOFF_GPIO_PORT             GPIOF
#define ONOFF_GPIO_MODE             GPIO_MODE_OUTPUT_OD
#define ONOFF_GPIO_PULL             GPIO_NOPULL
#define ONOFF_GPIO_SPEED            GPIO_SPEED_FREQ_VERY_HIGH
#define ONOFF_GPIO_CLK              gpio_f_clk_en

#define SYS_NRST_GPIO_PIN           GPIO_PIN_5
#define SYS_NRST_GPIO_PORT          GPIOF
#define SYS_NRST_GPIO_MODE          GPIO_MODE_OUTPUT_OD
#define SYS_NRST_GPIO_PULL          GPIO_NOPULL
#define SYS_NRST_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define SYS_NRST_GPIO_CLK           gpio_f_clk_en

#define MCB_EN_GPIO_PIN             GPIO_PIN_11
#define MCB_EN_GPIO_PORT            GPIOF
#define MCB_EN_GPIO_MODE            GPIO_MODE_OUTPUT_PP
#define MCB_EN_GPIO_PULL            GPIO_NOPULL
#define MCB_EN_GPIO_SPEED           GPIO_SPEED_FREQ_VERY_HIGH
#define MCB_EN_GPIO_CLK             gpio_f_clk_en

#define EXT12V_EN_GPIO_PIN          GPIO_PIN_13
#define EXT12V_EN_GPIO_PORT         GPIOF
#define EXT12V_EN_GPIO_MODE         GPIO_MODE_OUTPUT_PP
#define EXT12V_EN_GPIO_PULL         GPIO_NOPULL
#define EXT12V_EN_GPIO_SPEED        GPIO_SPEED_FREQ_VERY_HIGH
#define EXT12V_EN_GPIO_CLK          gpio_f_clk_en

//GPIO_G OUTPUT
#define EXT_EN_GPIO_PIN             GPIO_PIN_5
#define EXT_EN_GPIO_PORT            GPIOG
#define EXT_EN_GPIO_MODE            GPIO_MODE_OUTPUT_PP
#define EXT_EN_GPIO_PULL            GPIO_NOPULL
#define EXT_EN_GPIO_SPEED           GPIO_SPEED_FREQ_VERY_HIGH
#define EXT_EN_GPIO_CLK             gpio_g_clk_en

#define LCB_RS485M_EN_GPIO_PIN      GPIO_PIN_6
#define LCB_RS485M_EN_GPIO_PORT     GPIOG
#define LCB_RS485M_EN_GPIO_MODE     GPIO_MODE_OUTPUT_PP
#define LCB_RS485M_EN_GPIO_PULL     GPIO_NOPULL
#define LCB_RS485M_EN_GPIO_SPEED    GPIO_SPEED_FREQ_VERY_HIGH
#define LCB_RS485M_EN_GPIO_CLK      gpio_g_clk_en


void hal_gpio_module_init();
void hal_input_gpio_init(input_gpio_list_num gpio_num);
void hal_output_gpio_init(output_gpio_list_num gpio_num);
void hal_gpio_set(output_gpio_list_num gpio_num, gpio_status set_gpio_status);

#endif //HAL_GPIO_H__