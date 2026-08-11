#include <stdbool.h>
#include <stdio.h>
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_key_handler.h"
#include "hal_power.h"
#include "hal_systick.h"
#include "mid_log.h"
#include "mid_scheduler.h"
#include "mid_tca8418_keypad.h"

static uint16_t mcu_recovery_key_push_timer = 0;
static bool start_mcu_recovery_key_push_timer = false;
void hal_mcu_recovery_key_handler(void)
{
    if(!start_mcu_recovery_key_push_timer) {
        return;
    }

    if(hal_gpio_get(MCU_RECOVERY_GPIO) == GPIO_LOW) {
        mcu_recovery_key_push_timer++;
        if(mcu_recovery_key_push_timer == 5000) {
            hal_flash_set_soc_recovery_flag(true);
            mid_log_print(LOG_VERBOSE, "set soc recovery flag\r\n");
            hal_set_soc_power_off();
            hal_set_soc_mcu_reset();
            start_mcu_recovery_key_push_timer = false;
        }
    }
    else {
        start_mcu_recovery_key_push_timer = false;
        mcu_recovery_key_push_timer = 0;
    }
}
void hal_mcu_recovery_key_it(void *p_event_data, uint16_t event_size)
{
    if(hal_gpio_get(MCU_RECOVERY_GPIO) == GPIO_LOW) {
        start_mcu_recovery_key_push_timer = true;
        mcu_recovery_key_push_timer = 0;
        mid_log_print(LOG_VERBOSE, "mcu recovery key push\r\n");
    }
    else {
        start_mcu_recovery_key_push_timer = false;
        mcu_recovery_key_push_timer = 0;
        mid_log_print(LOG_VERBOSE, "mcu recovery key release\r\n");
    }
}

#define KEYPAD_ROWS ((uint8_t)2)
#define KEYPAD_COLS ((uint8_t)6)

uint8_t hal_keypad_get_rows(void)
{
    return KEYPAD_ROWS;
}

uint8_t hal_keypad_get_cols(void)
{
    return KEYPAD_COLS;
}

void hal_keypad_hw_reset(void)
{
    hal_gpio_set(MCU_KEY_REST_GPIO, GPIO_LOW);
    hal_delay_1ms(1);
    hal_gpio_set(MCU_KEY_REST_GPIO, GPIO_HIGH);
    hal_delay_1ms(100);
}

void hal_keypad_it(void *p_event_data, uint16_t event_size)
{
    mid_tca8418_keypad_read_handler();
}

void hal_keypad_it_reset(void)
{
    hal_input_gpio_init(MCU_KEY_INT_GPIO);
}

uint8_t hal_keypad_i2c_read(uint32_t addr, uint8_t read_address, uint8_t *data, uint16_t len)
{
    return hal_i2c_read(I2C_ID_0, addr, read_address, data, len);
}

uint8_t hal_keypad_i2c_write(uint32_t addr, uint8_t write_address, uint8_t data, uint16_t len)
{
    return hal_i2c_write(I2C_ID_0, addr, write_address, &data, len);
}