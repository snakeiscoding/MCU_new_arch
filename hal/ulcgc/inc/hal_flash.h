#ifndef HAL_FLASH_H__
#define HAL_FLASH_H__

#include <stdbool.h>
#include <stdint.h>

bool hal_flash_erase_fw_update_partition(void);
bool hal_flash_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len);
void hal_flash_read_fw_update_data(uint32_t start_addr, uint8_t *data, uint16_t len);
void hal_flash_set_fw_update_data_size(uint32_t fw_size);
void hal_flash_set_fw_update_ready_flag(bool flag);
void hal_flash_set_soc_recovery_flag(bool flag);
bool hal_flash_get_soc_recovery_flag(void);
void hal_flash_clear_soc_recovery_flag(void);
void hal_flash_init(void);

#endif //HAL_FLASH_H__