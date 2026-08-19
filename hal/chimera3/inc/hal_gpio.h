#ifndef HAL_GPIO_H__
#define HAL_GPIO_H__

#include "stm32f4xx.h"

typedef enum {
    FAN_RPM_GPIO = 0,
    KEYV_D1_GPIO,
    KEYV_D2_GPIO,
    SAFETY_KEY_GPIO,
    INPUT_GPIO_TOTAL_NUM,
} input_gpio_list_num;

/* Inputs that are polled rather than interrupt driven carry this instead of an
 * EXTI line, so hal_input_gpio_init() knows to skip the NVIC setup. */
#define GPIO_EXTI_NONE  ((uint16_t)0xFFFF)

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
    KEYH_D1_GPIO,
    KEYH_D2_GPIO,
    KEYH_D3_GPIO,
    KEYH_D4_GPIO,
    KEYH_D5_GPIO,
    KEYH_D6_GPIO,
    CSAFE_PWR_EN_GPIO,
    RFID_5V_EN_GPIO,
    TV_12V_EN_GPIO,
    HR_5V_EN_GPIO,
    QI_5W_EN_GPIO,
    QI_12W_EN_GPIO,
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

/* Single-pin power rails, all active high and all enabled at boot - the legacy
 * PwrFlowCtrl[] brings every one of them up with GPIO_PIN_SET, and its runtime
 * switch (the generic branch at the end of PwrPinCtrl()) writes the requested
 * enable bit straight to the pin. Csafe, RFID 5V, tuner 12V and heart rate 5V
 * need nothing beyond that one write; Qi and Ext 12V are the two that do, and
 * they are handled separately.
 *
 * Note PB4 doubles as NJTRST. That is a JTAG-only alternate function, not SWD,
 * so driving it as an output leaves the SWD debug port intact - the legacy
 * drove this same pin as Csafe_PWR_EN. */
//GPIO_B OUTPUT
#define CSAFE_PWR_EN_GPIO_PIN       GPIO_PIN_4
#define CSAFE_PWR_EN_GPIO_PORT      GPIOB
#define CSAFE_PWR_EN_GPIO_MODE      GPIO_MODE_OUTPUT_PP
#define CSAFE_PWR_EN_GPIO_PULL      GPIO_NOPULL
#define CSAFE_PWR_EN_GPIO_SPEED     GPIO_SPEED_FREQ_VERY_HIGH
#define CSAFE_PWR_EN_GPIO_CLK       gpio_b_clk_en

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

/* Matrix keypad, 6 drive lines x 2 sense lines = 12 keys.
 *
 * The drive lines idle high; a scan pulls one low at a time and a pressed key
 * pulls the corresponding sense line low with it. Same topology and the same
 * pins as the legacy MatrixKey.c.
 *
 * The sense lines are given an internal pull-up. The legacy left every input at
 * GPIO_NOPULL and relied on the board's own pull-ups (its GPIOE pull-up block
 * is commented out), but for a drive-low matrix a pull-up is the correct idle
 * state and an internal one simply parallels an external one. If the board
 * turns out to need the legacy's exact configuration, change these two to
 * GPIO_NOPULL. */
#define KEYH_D1_GPIO_PIN            GPIO_PIN_7
#define KEYH_D1_GPIO_PORT           GPIOE
#define KEYH_D1_GPIO_MODE           GPIO_MODE_OUTPUT_PP
#define KEYH_D1_GPIO_PULL           GPIO_NOPULL
#define KEYH_D1_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYH_D1_GPIO_CLK            gpio_e_clk_en

#define KEYH_D2_GPIO_PIN            GPIO_PIN_8
#define KEYH_D2_GPIO_PORT           GPIOE
#define KEYH_D2_GPIO_MODE           GPIO_MODE_OUTPUT_PP
#define KEYH_D2_GPIO_PULL           GPIO_NOPULL
#define KEYH_D2_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYH_D2_GPIO_CLK            gpio_e_clk_en

#define KEYH_D3_GPIO_PIN            GPIO_PIN_9
#define KEYH_D3_GPIO_PORT           GPIOE
#define KEYH_D3_GPIO_MODE           GPIO_MODE_OUTPUT_PP
#define KEYH_D3_GPIO_PULL           GPIO_NOPULL
#define KEYH_D3_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYH_D3_GPIO_CLK            gpio_e_clk_en

#define KEYH_D4_GPIO_PIN            GPIO_PIN_10
#define KEYH_D4_GPIO_PORT           GPIOE
#define KEYH_D4_GPIO_MODE           GPIO_MODE_OUTPUT_PP
#define KEYH_D4_GPIO_PULL           GPIO_NOPULL
#define KEYH_D4_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYH_D4_GPIO_CLK            gpio_e_clk_en

#define KEYH_D5_GPIO_PIN            GPIO_PIN_11
#define KEYH_D5_GPIO_PORT           GPIOE
#define KEYH_D5_GPIO_MODE           GPIO_MODE_OUTPUT_PP
#define KEYH_D5_GPIO_PULL           GPIO_NOPULL
#define KEYH_D5_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYH_D5_GPIO_CLK            gpio_e_clk_en

#define KEYH_D6_GPIO_PIN            GPIO_PIN_12
#define KEYH_D6_GPIO_PORT           GPIOE
#define KEYH_D6_GPIO_MODE           GPIO_MODE_OUTPUT_PP
#define KEYH_D6_GPIO_PULL           GPIO_NOPULL
#define KEYH_D6_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYH_D6_GPIO_CLK            gpio_e_clk_en

//GPIO_E INPUT (polled)
#define KEYV_D1_GPIO_PIN            GPIO_PIN_13
#define KEYV_D1_GPIO_PORT           GPIOE
#define KEYV_D1_GPIO_MODE           GPIO_MODE_INPUT
#define KEYV_D1_GPIO_PULL           GPIO_PULLUP
#define KEYV_D1_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYV_D1_GPIO_EXTI           GPIO_EXTI_NONE
#define KEYV_D1_GPIO_CLK            gpio_e_clk_en

#define KEYV_D2_GPIO_PIN            GPIO_PIN_14
#define KEYV_D2_GPIO_PORT           GPIOE
#define KEYV_D2_GPIO_MODE           GPIO_MODE_INPUT
#define KEYV_D2_GPIO_PULL           GPIO_PULLUP
#define KEYV_D2_GPIO_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define KEYV_D2_GPIO_EXTI           GPIO_EXTI_NONE
#define KEYV_D2_GPIO_CLK            gpio_e_clk_en

/* Safety key / E-Stop (the legacy MCU_SW_POSITION). Low means triggered.
 * Polled here rather than run off EXTI as the legacy did - its handler used a
 * blocking HAL_Delay(10) inside the ISR to debounce, which this architecture
 * gets for free from the 1ms tick. */
//GPIO_A INPUT (polled)
#define SAFETY_KEY_GPIO_PIN         GPIO_PIN_6
#define SAFETY_KEY_GPIO_PORT        GPIOA
#define SAFETY_KEY_GPIO_MODE        GPIO_MODE_INPUT
#define SAFETY_KEY_GPIO_PULL        GPIO_NOPULL
#define SAFETY_KEY_GPIO_SPEED       GPIO_SPEED_FREQ_VERY_HIGH
#define SAFETY_KEY_GPIO_EXTI        GPIO_EXTI_NONE
#define SAFETY_KEY_GPIO_CLK         gpio_a_clk_en

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

#define HR_5V_EN_GPIO_PIN           GPIO_PIN_3
#define HR_5V_EN_GPIO_PORT          GPIOD
#define HR_5V_EN_GPIO_MODE          GPIO_MODE_OUTPUT_PP
#define HR_5V_EN_GPIO_PULL          GPIO_NOPULL
#define HR_5V_EN_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define HR_5V_EN_GPIO_CLK           gpio_d_clk_en

#define TV_12V_EN_GPIO_PIN          GPIO_PIN_7
#define TV_12V_EN_GPIO_PORT         GPIOD
#define TV_12V_EN_GPIO_MODE         GPIO_MODE_OUTPUT_PP
#define TV_12V_EN_GPIO_PULL         GPIO_NOPULL
#define TV_12V_EN_GPIO_SPEED        GPIO_SPEED_FREQ_VERY_HIGH
#define TV_12V_EN_GPIO_CLK          gpio_d_clk_en

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

#define RFID_5V_EN_GPIO_PIN         GPIO_PIN_15
#define RFID_5V_EN_GPIO_PORT        GPIOF
#define RFID_5V_EN_GPIO_MODE        GPIO_MODE_OUTPUT_PP
#define RFID_5V_EN_GPIO_PULL        GPIO_NOPULL
#define RFID_5V_EN_GPIO_SPEED       GPIO_SPEED_FREQ_VERY_HIGH
#define RFID_5V_EN_GPIO_CLK         gpio_f_clk_en

/* Qi charging rail selection. Two P-FET load switches (Q78/Q79 and Q80/Q81)
 * both feed the single QI_POWER net: QI_12W_EN gates +12V_QI (15W), QI_5W_EN
 * gates +5V_SW (5W, produced by the U94 buck off +12V_SW). Both are active
 * high, and because they share the output net only one may ever be on - see
 * hal_soc_cmd_interface.c. Boot state is 5W on, 12W off, as PwrFlowCtrl[] had
 * it. There is no 9V rail on this board. */
#define QI_5W_EN_GPIO_PIN           GPIO_PIN_6
#define QI_5W_EN_GPIO_PORT          GPIOF
#define QI_5W_EN_GPIO_MODE          GPIO_MODE_OUTPUT_PP
#define QI_5W_EN_GPIO_PULL          GPIO_NOPULL
#define QI_5W_EN_GPIO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define QI_5W_EN_GPIO_CLK           gpio_f_clk_en

#define QI_12W_EN_GPIO_PIN          GPIO_PIN_7
#define QI_12W_EN_GPIO_PORT         GPIOF
#define QI_12W_EN_GPIO_MODE         GPIO_MODE_OUTPUT_PP
#define QI_12W_EN_GPIO_PULL         GPIO_NOPULL
#define QI_12W_EN_GPIO_SPEED        GPIO_SPEED_FREQ_VERY_HIGH
#define QI_12W_EN_GPIO_CLK          gpio_f_clk_en

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
gpio_status hal_gpio_get(input_gpio_list_num gpio_num);

#endif //HAL_GPIO_H__