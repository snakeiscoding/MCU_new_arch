#ifndef HAL_FLASH_H__
#define HAL_FLASH_H__

#include <stdbool.h>
#include <stdint.h>

/*
 * chimera3 interoperates with the existing legacy IAP: the AP only latches
 * GoToMode and resets, and the IAP then hosts the whole update session. Under
 * that model the image is staged in SRAM and committed to the AP partition in
 * one shot at End Update - see hal_flash.c for the full sequence.
 *
 * The erase/write entry points below are therefore only meaningful in a
 * MODE=boot build, which does not exist yet. In the MODE=app build they
 * refuse, because the AP partition is the running image.
 */
bool hal_flash_erase_fw_update_partition(void);
bool hal_flash_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len);
void hal_flash_read_fw_update_data(uint32_t start_addr, uint8_t *data, uint16_t len);

/* GoToMode: the latch the IAP checks on reset to decide whether to run the
 * update session or boot straight into the AP. Stored inside the legacy
 * _INFOCTL block so the existing IAP reads it unchanged. */
bool hal_flash_set_goto_iap_mode(bool enable);
bool hal_flash_get_goto_iap_mode(void);

/* No staging hand-off in this model, and no recovery key on this board. Kept
 * so the shared firmware-update interface links the same way on every board. */
void hal_flash_set_fw_update_data_size(uint32_t fw_size);
void hal_flash_set_fw_update_ready_flag(bool flag);
void hal_flash_set_soc_recovery_flag(bool flag);
bool hal_flash_get_soc_recovery_flag(void);
void hal_flash_clear_soc_recovery_flag(void);
void hal_flash_init(void);

#endif //HAL_FLASH_H__
