#include <stdbool.h>
#include <stdio.h>
#include "app_soc_cmd_control_func.h"
#include "app_soc_cmd.h"
#include "hal_key_handler.h"
#include "mid_log.h"
#include "mid_scheduler.h"
#include "mid_soc_cmd.h"
#include "mid_tca8418_keypad.h"

#define BIT(x)  ((uint32_t)((uint32_t)0x01U<<(x)))

/* TCA8418 hardware limits */
#define TCA8418_MAX_ROWS 8
#define TCA8418_MAX_COLS 10

/* TCA8418 register offsets */
#define REG_CFG             0x01
#define REG_INT_STAT        0x02
#define REG_KEY_LCK_EC      0x03
#define REG_KEY_EVENT_A     0x04
#define REG_KEY_EVENT_B     0x05
#define REG_KEY_EVENT_C     0x06
#define REG_KEY_EVENT_D     0x07
#define REG_KEY_EVENT_E     0x08
#define REG_KEY_EVENT_F     0x09
#define REG_KEY_EVENT_G     0x0A
#define REG_KEY_EVENT_H     0x0B
#define REG_KEY_EVENT_I     0x0C
#define REG_KEY_EVENT_J     0x0D
#define REG_KP_LCK_TIMER    0x0E
#define REG_UNLOCK1         0x0F
#define REG_UNLOCK2         0x10
#define REG_GPIO_INT_STAT1  0x11
#define REG_GPIO_INT_STAT2  0x12
#define REG_GPIO_INT_STAT3  0x13
#define REG_GPIO_DAT_STAT1  0x14
#define REG_GPIO_DAT_STAT2  0x15
#define REG_GPIO_DAT_STAT3  0x16
#define REG_GPIO_DAT_OUT1   0x17
#define REG_GPIO_DAT_OUT2   0x18
#define REG_GPIO_DAT_OUT3   0x19
#define REG_GPIO_INT_EN1    0x1A
#define REG_GPIO_INT_EN2    0x1B
#define REG_GPIO_INT_EN3    0x1C
#define REG_KP_GPIO1        0x1D
#define REG_KP_GPIO2        0x1E
#define REG_KP_GPIO3        0x1F
#define REG_GPI_EM1         0x20
#define REG_GPI_EM2         0x21
#define REG_GPI_EM3         0x22
#define REG_GPIO_DIR1       0x23
#define REG_GPIO_DIR2       0x24
#define REG_GPIO_DIR3       0x25
#define REG_GPIO_INT_LVL1   0x26
#define REG_GPIO_INT_LVL2   0x27
#define REG_GPIO_INT_LVL3   0x28
#define REG_DEBOUNCE_DIS1   0x29
#define REG_DEBOUNCE_DIS2   0x2A
#define REG_DEBOUNCE_DIS3   0x2B
#define REG_GPIO_PULL1      0x2C
#define REG_GPIO_PULL2      0x2D
#define REG_GPIO_PULL3      0x2E

/* TCA8418 bit definitions */
#define CFG_AI              BIT(7)
#define CFG_GPI_E_CFG       BIT(6)
#define CFG_OVR_FLOW_M      BIT(5)
#define CFG_INT_CFG         BIT(4)
#define CFG_OVR_FLOW_IEN    BIT(3)
#define CFG_K_LCK_IEN       BIT(2)
#define CFG_GPI_IEN         BIT(1)
#define CFG_KE_IEN          BIT(0)

#define INT_STAT_CAD_INT        BIT(4)
#define INT_STAT_OVR_FLOW_INT   BIT(3)
#define INT_STAT_K_LCK_INT      BIT(2)
#define INT_STAT_GPI_INT        BIT(1)
#define INT_STAT_K_INT          BIT(0)

/* TCA8418 register masks */
#define KEY_LCK_EC_KEC  0x7
#define KEY_EVENT_CODE  0x7f
#define KEY_EVENT_VALUE 0x80

#define TCA8418_SLAVE_ADDRESS (uint8_t)0x34

/* Key Mapping:
- 0x01 : Incline Up
- 0x02 : Incline Down
- 0x03 : 
- 0x04 : 
- 0x05 : Burning Key 1
- 0x06 : Cool Down
- 0x0B : Speed Up
- 0x0C : Speed Down
- 0x0D : Pause
- 0x0E : Go
- 0x0F : 
- 0x10 : Burning Key 2 */
bool mid_tca8418_keypad_rawdata_convert(uint8_t key_num, uint8_t *convert_row, uint8_t *convert_col)
{
    bool result = true;
    switch(key_num) {
    case 0x01:
        *convert_row = 0x01;
        *convert_col = 0x01;
        break;
    case 0x02:
        *convert_row = 0x01;
        *convert_col = 0x02;
        break;
    case 0x05:
        *convert_row = 0x01;
        *convert_col = 0x10;
        break;
    case 0x06:
        *convert_row = 0x01;
        *convert_col = 0x20;
        break;
    case 0x0B:
        *convert_row = 0x02;
        *convert_col = 0x01;
        break;
    case 0x0C:
        *convert_row = 0x02;
        *convert_col = 0x02;
        break;
    case 0x0D:
        *convert_row = 0x02;
        *convert_col = 0x04;
        break;
    case 0x0E:
        *convert_row = 0x02;
        *convert_col = 0x08;
        break;
    case 0x10:
        *convert_row = 0x02;
        *convert_col = 0x20;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void mid_tca8418_keypad_event(void *p_event_data, uint16_t event_size)
{
    uint8_t *data = (uint8_t *)p_event_data;
    mid_log_print(LOG_INFO, "tca8418 keypad raw data row : %d, col : %d, state : %d, key_num : %d \r\n", data[2], data[3], !data[0], data[1]);

    uint8_t cmd_data[3];
    if(mid_tca8418_keypad_rawdata_convert(data[1], &data[2], &data[3])) {
        cmd_data[0] = !data[0];
        cmd_data[1] = data[3];
        cmd_data[2] = data[2];
    }
    else {
        cmd_data[0] = !data[0];
        cmd_data[1] = 0;
        cmd_data[2] = data[1];
    }
    mid_soc_cmd_send(CMD_EVENT_KEY, CMD_RESULT_CODE_SUCCESS, cmd_data, 3);
    
    if(data[0]) {
        app_control_tone(0x01);
    }
}

uint8_t mid_tca8418_clear_all_interrupt(void)
{
    return hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_INT_STAT, 0xFF, 1);
}

__attribute__((weak)) uint8_t hal_keypad_i2c_read(uint32_t addr, uint8_t read_address, uint8_t *data, uint16_t len) {return 0;}
__attribute__((weak)) uint8_t hal_keypad_i2c_write(uint32_t addr, uint8_t write_address, uint8_t data, uint16_t len) {return 0;}
void mid_tca8418_keypad_read(void)
{
    uint8_t reg;
    uint8_t data[4];
    hal_keypad_i2c_read(TCA8418_SLAVE_ADDRESS, REG_KEY_EVENT_A, &reg, 1);

    while (reg > 0) {
        data[0] = (reg & KEY_EVENT_VALUE) >> 7;
        data[1] = reg & KEY_EVENT_CODE;
        data[2] = data[1] / TCA8418_MAX_COLS;
        data[3] = data[1] % TCA8418_MAX_COLS;
        mid_sched_event_put((void *)data, 4, mid_tca8418_keypad_event);

        hal_keypad_i2c_read(TCA8418_SLAVE_ADDRESS, REG_KEY_EVENT_A, &reg, 1);
    }
}
bool tca8418_init_flag = false;
void mid_tca8418_keypad_read_handler(void)
{
    if(!tca8418_init_flag) {
        return;
    }

    uint8_t reg, ret;
    ret = hal_keypad_i2c_read(TCA8418_SLAVE_ADDRESS, REG_INT_STAT, &reg, 1);
    if(ret > 0) {
        mid_log_print(LOG_ERROR, "tca8418 read REG_INT_STAT error!\r\n");
    }

    if(!reg) {
        return;
    }

    if(reg & INT_STAT_OVR_FLOW_INT) {
        mid_log_print(LOG_VERBOSE, "overflow occurred\r\n");
    }

    if(reg & INT_STAT_K_INT) {
        mid_tca8418_keypad_read();
    }

    ret = mid_tca8418_clear_all_interrupt();
    if(ret > 0) {
        mid_log_print(LOG_ERROR, "tca8418 write REG_INT_STAT error!\r\n");
    }
}

void mid_tca8418_keypad_configure(uint8_t rows, uint8_t cols)
{
	uint32_t reg, ret;

    ret = hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_CFG, CFG_INT_CFG | CFG_OVR_FLOW_IEN | CFG_KE_IEN, 1);
    if(ret > 0) {
        mid_log_print(LOG_ERROR, "tca8418 write REG_CFG error!\r\n");
    }

    reg = ~(~0 << rows);
    reg += (~(~0 << cols)) << 8;

    /* Set registers to keypad mode */
    ret |= hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_KP_GPIO1, reg, 1);
    ret |= hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_KP_GPIO2, reg >> 8, 1);
    ret |= hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_KP_GPIO3, reg >> 16, 1);
    if(ret > 0) {
        mid_log_print(LOG_ERROR, "tca8418 write REG_KP_GPIO error!\r\n");
    }

    /* Enable column debouncing */
    ret |= hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_DEBOUNCE_DIS1, reg, 1);
    ret |= hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_DEBOUNCE_DIS2, reg >> 8, 1);
    ret |= hal_keypad_i2c_write(TCA8418_SLAVE_ADDRESS, REG_DEBOUNCE_DIS3, reg >> 16, 1);
    if(ret > 0) {
        mid_log_print(LOG_ERROR, "tca8418 write REG_DEBOUNCE_DIS error!\r\n");
    }
}

__attribute__((weak)) void hal_keypad_hw_reset(void) {}
__attribute__((weak)) void hal_keypad_it_reset(void) {}
__attribute__((weak)) uint8_t hal_keypad_get_rows(void) {return 0;}
__attribute__((weak)) uint8_t hal_keypad_get_cols(void) {return 0;}
extern volatile uint64_t time_1ms;
volatile uint64_t delay_timer = 0;

void mid_tca8418_keypad_init(void)
{
    if(tca8418_init_flag) {
        return;
    }
    if(time_1ms - delay_timer > 8000) {
        hal_keypad_hw_reset();
        mid_tca8418_keypad_configure(hal_keypad_get_rows(), hal_keypad_get_cols());
        hal_keypad_it_reset();
        mid_tca8418_clear_all_interrupt();
        tca8418_init_flag =  true;
        mid_log_print(LOG_INFO, "tca8418 init ready\r\n");
    }
}

void mid_tca8418_reset_keypad_init(void)
{
    delay_timer = time_1ms;
    tca8418_init_flag = false;
    mid_log_print(LOG_INFO, "tca8418 init reset\r\n");
}