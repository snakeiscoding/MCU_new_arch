#ifndef HAL_KEY_HANDLER_H__
#define HAL_KEY_HANDLER_H__

#include <stdint.h>

void hal_mcu_reset_key_handler(void);
void hal_mcu_reset_key_it(void *p_event_data, uint16_t event_size);
void hal_mcu_recovery_key_handler(void);
void hal_mcu_recovery_key_it(void *p_event_data, uint16_t event_size);

uint8_t hal_keypad_get_rows(void);
uint8_t hal_keypad_get_cols(void);
void hal_keypad_hw_reset(void);
void hal_keypad_it(void *p_event_data, uint16_t event_size);
void hal_keypad_it_reset(void);
uint8_t hal_keypad_i2c_read(uint32_t addr, uint8_t read_address, uint8_t *data, uint16_t len);
uint8_t hal_keypad_i2c_write(uint32_t addr, uint8_t write_address, uint8_t data, uint16_t len);

#endif //HAL_KEY_HANDLER_H__