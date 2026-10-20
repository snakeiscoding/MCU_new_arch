#include <stdbool.h>
#include "hal_erp_handler.h"
#include "hal_gpio.h"
#include "hal_ir.h"
#include "hal_stm32f4xx_it.h"
#include "mid_log.h"
#include "mid_soc_cmd.h"
#include "stm32f4xx.h"

extern volatile uint64_t time_1ms;

/* STB_IR is not in the hal_gpio.c output table: it switches between plain
 * output (idle, and the spaces) and TIM5 CH2 alternate function (the marks)
 * while a frame is going out, exactly as the legacy SetIREmitter() did. */
#define STB_IR_GPIO_PIN     GPIO_PIN_1
#define STB_IR_GPIO_PORT    GPIOA

TIM_HandleTypeDef Tim7Handle;
static TIM_HandleTypeDef Tim5Handle;
static TIM_OC_InitTypeDef Tim5OutConfig;

/* ---------------------------------------------------------------------------
 * Transmit - a straight port of the legacy IRCtrl.c.
 *
 * Clock tree is the legacy one (168MHz core, APB1 timers at 84MHz), so the
 * register values below come out the same as they did there:
 *   TIM7 prescaler 83 -> 1MHz, one tick per microsecond.
 *   TIM5 prescaler 1  -> 42MHz, period SystemCoreClock / (freq * 1000).
 *
 * LEGACY QUIRK (carrier): with that prescaler the carrier is freq * 250Hz,
 * not freq kHz - freq = 38 gives 9.5kHz. Ported unchanged; measure STB_IR
 * before relying on it. */

static uint16_t ir_timing_data[512];
static volatile uint16_t ir_bit_buffer_len = 0;
static volatile uint16_t ir_bit_buffer_index = 0;

static void ir_stb_gpio_output(GPIO_PinState level)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = STB_IR_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(STB_IR_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(STB_IR_GPIO_PORT, STB_IR_GPIO_PIN, level);
}

/* Legacy IRPWMReinit(). freq 0 keeps a 20kHz period with a 0% pulse, so the
 * "marks" of such a frame are a steady low with no carrier. */
static void ir_pwm_reinit(uint16_t frequency)
{
    HAL_TIM_PWM_Stop(&Tim5Handle, TIM_CHANNEL_2);

    if(frequency == 0) {
        frequency = 20;
        Tim5Handle.Init.Period = (SystemCoreClock / (20 * 1000 * (1))) - 1;
        Tim5OutConfig.Pulse = 0;
    }
    else {
        Tim5Handle.Init.Period = (SystemCoreClock / (frequency * 1000 * (1))) - 1;
        Tim5OutConfig.Pulse = Tim5Handle.Init.Period / 2;
    }
    Tim5Handle.Instance = TIM5;
    Tim5Handle.Init.Prescaler = 1;
    Tim5Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim5Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    Tim5Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_PWM_Init(&Tim5Handle) != HAL_OK) {
        Error_Handler();
    }

    Tim5OutConfig.OCMode = TIM_OCMODE_PWM1;
    Tim5OutConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim5OutConfig.OCFastMode = TIM_OCFAST_DISABLE;
    if(HAL_TIM_PWM_ConfigChannel(&Tim5Handle, &Tim5OutConfig, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    __HAL_TIM_DISABLE_OCxPRELOAD(&Tim5Handle, TIM_CHANNEL_2);
}

/* Legacy SetIREmitter(): on = carrier out of TIM5, off = STB_IR driven high. */
static void ir_set_emitter(bool enable_pwm)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    HAL_TIM_PWM_Stop(&Tim5Handle, TIM_CHANNEL_2);
    if(enable_pwm) {
        GPIO_InitStruct.Pin = STB_IR_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
        HAL_GPIO_Init(STB_IR_GPIO_PORT, &GPIO_InitStruct);
        HAL_TIM_PWM_Start(&Tim5Handle, TIM_CHANNEL_2);
    }
    else {
        ir_stb_gpio_output(GPIO_PIN_SET);
    }
}

/* Legacy IRTimerReinit(): the next TIM7 update fires period + 1 us from now. */
static void ir_timer_reinit(uint16_t period)
{
    __HAL_TIM_SET_AUTORELOAD(&Tim7Handle, period);
    __HAL_TIM_SET_COUNTER(&Tim7Handle, 0);
    __HAL_TIM_CLEAR_IT(&Tim7Handle, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&Tim7Handle);
}

/* Legacy ExecuteIRCommand(), run from the TIM7 interrupt. Entries alternate
 * space (even index) and mark (odd index), and the line ends high. */
static void ir_execute_command(void)
{
    if(ir_bit_buffer_len > 0) {
        ir_set_emitter((ir_bit_buffer_index % 2) != 0);
        ir_bit_buffer_index++;
        ir_bit_buffer_len--;
        ir_timer_reinit(ir_timing_data[ir_bit_buffer_index - 1]);
    }
    else {
        HAL_TIM_Base_Stop_IT(&Tim7Handle);
        HAL_TIM_PWM_Stop(&Tim5Handle, TIM_CHANNEL_2);
        ir_stb_gpio_output(GPIO_PIN_SET);
        ir_bit_buffer_index = 0;
        ir_bit_buffer_len = 0;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &Tim7Handle) {
        HAL_TIM_Base_Stop_IT(&Tim7Handle);
        ir_execute_command();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM7) {
        __HAL_RCC_TIM7_CLK_ENABLE();
        /* Same priority as the legacy: above the EXTI and UART lines, below
         * SysTick. */
        HAL_NVIC_SetPriority(TIM7_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(TIM7_IRQn);
    }
}

/* Legacy SendIRCommand(). Payload:
 *
 *   [0]  freq          carrier, see the quirk note at the top; 0 = no carrier
 *   [1]  header_len    number of 16-bit header entries
 *   [2]  header_repeat header is sent header_repeat + 1 times
 *   [3]  header        header_len entries, little endian
 *   ...  data_len, data_repeat, data entries - but see quirk 1
 *
 * Each entry n becomes n * 500 / freq microseconds, or n * 8 when freq is 0,
 * truncated to 16 bits. The table is then played as space, mark, space, ...
 * after a 1ms low lead-in.
 *
 * The parsing below is the legacy one, quirks included, so the line can be
 * compared against a legacy board byte for byte:
 *   1. data_len and data_repeat are read at 3 + header_len and 4 + header_len,
 *      not 3 + 2 * header_len - with header_len > 0 they land inside the
 *      header entries.
 *   2. Scaling reads ir_timing_data[w] instead of the entry just stored, so
 *      only the first header pass comes out right; repeats and data entries
 *      repeat already scaled header values.
 *   3. Data entries stop at 256 table slots, header entries at 512.
 *   4. An entry that scales to 0 loads TIM7 with ARR 0, which never updates,
 *      so the frame never finishes and every later command is refused as busy.
 *
 * Added here: the length check, since the legacy read past the end of a short
 * payload. Everything else, including acking a command dropped as busy with
 * SUCCESS (the legacy acked every IR command with 0x0000), is unchanged. */
uint16_t hal_send_ir_command(uint8_t *data, uint16_t len)
{
    mid_log_print(LOG_VERBOSE, "%s\r\n", __FUNCTION__);

    if(len < 3 || len < (3 + 2 * (uint32_t)data[1])) {
        return CMD_RESULT_CODE_INVALID_PAYLOAD_LENGTH;
    }
    if(len < 5 + (uint32_t)data[1]) {
        return CMD_RESULT_CODE_INVALID_PAYLOAD_LENGTH;
    }
    if(len < 5 + (uint32_t)data[1] + 2 * (uint32_t)data[3 + data[1]]) {
        return CMD_RESULT_CODE_INVALID_PAYLOAD_LENGTH;
    }

    if(ir_bit_buffer_len) {
        mid_log_print(LOG_INFO, "IR: previous frame not finished, command dropped\r\n");
        return CMD_RESULT_CODE_SUCCESS;
    }

    uint16_t freq = data[0];
    uint16_t header_len = data[1];
    uint16_t header_repeat = data[2];
    uint8_t *header = &data[3];

    uint16_t body_len = data[3 + header_len];
    uint16_t body_repeat = data[4 + header_len];
    uint8_t *body = &data[5 + header_len];

    for(int q = 0; q <= header_repeat; q++) {
        for(int w = 0; w < header_len; w++) {
            if(ir_bit_buffer_len < 512) {
                ir_timing_data[ir_bit_buffer_len] = header[w * 2] + header[w * 2 + 1] * 256;
                if(freq != 0) {
                    ir_timing_data[ir_bit_buffer_len] = (ir_timing_data[w] * 500) / freq;
                }
                else {
                    ir_timing_data[ir_bit_buffer_len] = ir_timing_data[w] * 8;
                }
                ir_bit_buffer_len++;
            }
        }
    }

    for(int q = 0; q <= body_repeat; q++) {
        for(int w = 0; w < body_len; w++) {
            if(ir_bit_buffer_len < 256) {
                ir_timing_data[ir_bit_buffer_len] = body[w * 2] + body[w * 2 + 1] * 256;
                if(freq != 0) {
                    ir_timing_data[ir_bit_buffer_len] = (ir_timing_data[w] * 500) / freq;
                }
                else {
                    ir_timing_data[ir_bit_buffer_len] = ir_timing_data[w] * 8;
                }
                ir_bit_buffer_len++;
            }
        }
    }

    mid_log_print(LOG_INFO, "IR: sending %u entries, freq %u\r\n",
                  (unsigned int)ir_bit_buffer_len, (unsigned int)freq);

    ir_stb_gpio_output(GPIO_PIN_RESET);
    ir_pwm_reinit(freq);
    ir_timer_reinit(1000);

    return CMD_RESULT_CODE_SUCCESS;
}

static void hal_ir_tx_stop(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    HAL_TIM_Base_Stop_IT(&Tim7Handle);
    HAL_TIM_PWM_Stop(&Tim5Handle, TIM_CHANNEL_2);
    ir_stb_gpio_output(GPIO_PIN_SET);
    ir_bit_buffer_index = 0;
    ir_bit_buffer_len = 0;
    if(!primask) {
        __enable_irq();
    }
}

void hal_ir_deinit(void)
{
    hal_ir_tx_stop();
    HAL_GPIO_DeInit(STB_IR_GPIO_PORT, STB_IR_GPIO_PIN);
}

/* ---------------------------------------------------------------------------
 * Receive. The legacy printf'd every edge from the EXTI callback; an IR frame
 * is dozens of edges, so here the ISR only counts and the 1ms handler logs one
 * line once the line has been quiet for 100ms. */
#define IR_RX_LOG_QUIET_MS  ((uint64_t)100)

static volatile uint32_t ir_rx_edge_count = 0;
static volatile uint64_t ir_rx_last_edge_time = 0;

void hal_ir_rx_edge(void)
{
    ir_rx_edge_count++;
    ir_rx_last_edge_time = time_1ms;
    hal_erp_ir_wakeup_flag_set();
}

void hal_ir_handler_1ms(void)
{
    uint32_t edges;

    if(ir_rx_edge_count == 0) {
        return;
    }
    if((time_1ms - ir_rx_last_edge_time) < IR_RX_LOG_QUIET_MS) {
        return;
    }

    __disable_irq();
    edges = ir_rx_edge_count;
    ir_rx_edge_count = 0;
    __enable_irq();

    mid_log_print(LOG_INFO, "IR receiver: %lu edges, line now %s\r\n", (unsigned long)edges,
                  (hal_gpio_get(IR_IN_GPIO) == GPIO_LOW) ? "low" : "high");
}

/* Legacy Tim7_IRQHandler_Config() and Tim5_PWM_Config(). TIM5 has to be
 * initialised here even though every command reconfigures it: the first thing
 * ir_pwm_reinit() does is stop it, and that needs a live handle. Unlike the
 * legacy, STB_IR is then parked as an output at its idle high instead of being
 * left in alternate function with the channel off. */
void hal_ir_init(void)
{
    Tim7Handle.Instance = TIM7;
    Tim7Handle.Init.Period = 100;
    Tim7Handle.Init.Prescaler = 83;
    Tim7Handle.Init.ClockDivision = 0;
    Tim7Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim7Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_Base_Init(&Tim7Handle) != HAL_OK) {
        Error_Handler();
    }

    Tim5Handle.Instance = TIM5;
    Tim5Handle.Init.Prescaler = 8399;
    Tim5Handle.Init.Period = 1000;
    Tim5Handle.Init.ClockDivision = 0;
    Tim5Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    Tim5Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_PWM_Init(&Tim5Handle) != HAL_OK) {
        Error_Handler();
    }
    Tim5OutConfig.OCMode = TIM_OCMODE_PWM1;
    Tim5OutConfig.Pulse = 5000;
    Tim5OutConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    Tim5OutConfig.OCFastMode = TIM_OCFAST_DISABLE;
    if(HAL_TIM_PWM_ConfigChannel(&Tim5Handle, &Tim5OutConfig, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    hal_ir_tx_stop();
    ir_rx_edge_count = 0;
}
