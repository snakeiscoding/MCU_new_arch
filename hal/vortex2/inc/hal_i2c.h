#ifndef HAL_I2C_H__
#define HAL_I2C_H__

#include <stdint.h>

enum I2C_ID {
    I2C_ID_0,
};

#define I2C_RW_SUCCESS  (uint8_t)0
#define I2C_RW_FAIL     (uint8_t)1

void hal_i2c_init(void);
uint8_t hal_i2c_read(enum I2C_ID i2c_id, uint32_t addr, uint8_t read_address, uint8_t *data, uint16_t len);
uint8_t hal_i2c_write(enum I2C_ID i2c_id, uint32_t addr, uint8_t write_address, uint8_t *data, uint16_t len);

#endif //HAL_I2C_H__