/*
 * hal_flash.c - chimera3 / STM32F407ZET6
 *
 * Has to interoperate with the legacy chimera3 update model, which the
 * existing IAP already implements and which has been through validation.
 * That model is (chimera3io, CommonInternalDriver/Src/SocUart.c - one file
 * shared by both the legacy AP and the legacy IAP):
 *
 *   AP    Start Update  -> latch GoToMode in the INFO block, then leave the AP
 *   IAP   (on reset, GoToMode set) stays in the IAP and runs the SoC session
 *         Erase Code    -> touches no flash; only arms an internal flag
 *         Write Code    -> staged into SRAM1 (0x20000000), NOT into flash
 *         Program State -> no verification; unconditionally reports "updated"
 *         End Update    -> copies the running AP to APP_BACKUP_PARTITION,
 *                          commits SRAM1 -> APP_PARTITION in one shot, clears
 *                          GoToMode and resets back into the AP
 *
 * So the image is staged in RAM rather than in a flash partition, and the AP
 * partition is written exactly once, at End Update, after the old image has
 * been backed up. The erase/write entry points below are the AP-side
 * counterparts of those SoC commands; they refuse in the MODE=app build
 * because the AP partition is this build's own running image.
 *
 * A MODE=boot build does not exist yet. Whoever writes one has to reproduce
 * the sequence above - including the APP_BACKUP_PARTITION copy - or it will
 * not be a drop-in replacement for the IAP that is in the field today.
 *
 * All addresses come from hal_flash_map.h, which both builds share.
 */

#include <string.h>
#include "hal_flash.h"
#include "hal_flash_map.h"
#include "stm32f4xx.h"

/* An erase or a program leaves the flash caches holding stale lines, so both
 * have to be reset afterwards (see RM0090 3.5.2). */
static void flash_cache_reset(void)
{
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
}

static void flash_clear_errors(void)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/* HAL_FLASHEx_Erase wants a sector *number*, while HAL_FLASH_Program wants an
 * *address*. Deriving the number from the address keeps the two from ever
 * disagreeing - the map header stays the only place an address is written. */
static uint32_t flash_sector_of(uint32_t addr)
{
    if(addr < ADDR_FLASH_SECTOR_4) {
        return (addr - ADDR_FLASH_SECTOR_0) / SIZE_FLASH_SECTOR_16K;    /* sector 0-3 */
    }
    if(addr < ADDR_FLASH_SECTOR_5) {
        return FLASH_SECTOR_4;                                          /* the 64K one */
    }
    return FLASH_SECTOR_5 + ((addr - ADDR_FLASH_SECTOR_5) / SIZE_FLASH_SECTOR_128K);
}

static bool flash_erase_sector(uint32_t addr)
{
    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error = 0;
    HAL_StatusTypeDef status;

    erase_init.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase_init.Banks        = 0;                        /* only used for a mass erase */
    erase_init.Sector       = flash_sector_of(addr);
    erase_init.NbSectors    = 1;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;    /* VDD 2.7~3.6V -> 32-bit parallelism */

    HAL_FLASH_Unlock();
    flash_clear_errors();
    status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
    HAL_FLASH_Lock();
    flash_cache_reset();

    return (status == HAL_OK);
}

/* Programs word-at-a-time where the destination allows it, bytes at both ends.
 * Caller does not need to unlock; this handles it. */
static bool flash_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t i = 0;
    bool ok = true;

    HAL_FLASH_Unlock();
    flash_clear_errors();

    while((i < len) && (((addr + i) & 0x03U) != 0U)) {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, data[i]) != HAL_OK) {
            ok = false;
            goto done;
        }
        i++;
    }
    while((len - i) >= 4U) {
        uint32_t word;
        memcpy(&word, &data[i], sizeof(word));  /* the source is not guaranteed to be aligned */
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i, word) != HAL_OK) {
            ok = false;
            goto done;
        }
        i += 4U;
    }
    while(i < len) {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, data[i]) != HAL_OK) {
            ok = false;
            goto done;
        }
        i++;
    }

done:
    HAL_FLASH_Lock();
    flash_cache_reset();
    return ok;
}

/* ------------------------------------------------------------------------
 * INFO block
 *
 * Byte-compatible with the legacy _INFOCTL (MemoryFlashWriteRead.h). The IAP
 * reads GoToMode straight out of this layout, so the field order and the
 * packing are part of the contract with it - do not reorder.
 * ------------------------------------------------------------------------ */

#pragma pack(push, 1)
typedef struct {
    uint32_t iap_version;
    uint32_t iap_version_backup;
    uint32_t app_version;
    uint32_t app_version_backup;
    uint8_t  iap_error_cnt;
    uint8_t  app_error_cnt;
    uint8_t  goto_mode;             /* 0 or 0xFF = boot the AP; anything else = stay in the IAP */
    uint8_t  ethernet_mac[6];
    uint8_t  serial_no[12];
    uint8_t  assy[4];
    uint8_t  serial_no1[12];
    uint8_t  assy1[4];
    uint8_t  serial_no2[12];
} flash_info_t;
#pragma pack(pop)

_Static_assert(sizeof(flash_info_t) == 69, "flash_info_t must stay byte-compatible with the legacy _INFOCTL");

#define GOTO_MODE_RUN_AP    ((uint8_t)0x00)
#define GOTO_MODE_STAY_IAP  ((uint8_t)0x01)
#define FLASH_ERASED_BYTE   ((uint8_t)0xFF)

static void info_load(flash_info_t *info)
{
    memcpy(info, (const void *)INFO_PARTITION, sizeof(*info));
}

/* A sector erase wipes the MAC, serial numbers and versions along with the
 * flag, so the whole block is read out, modified and written back - the same
 * read-modify-write the legacy SetSystemParameter() did. The backup sector is
 * kept in step, again matching the legacy behaviour. */
static bool info_store(const flash_info_t *info)
{
    if(!flash_erase_sector(INFO_PARTITION)) {
        return false;
    }
    if(!flash_program(INFO_PARTITION, (const uint8_t *)info, sizeof(*info))) {
        return false;
    }
    if(!flash_erase_sector(INFO_BACKUP_PARTITION)) {
        return false;
    }
    // return flash_program(INFO_BACKUP_PARTITION, (const uint8_t *)info, sizeof(*info));
    if(!flash_program(INFO_BACKUP_PARTITION, (const uint8_t *)info, sizeof(*info))) {
        return false;
    }
    /* The legacy MemoryMoveFromSourToDest() compared source against destination
     * after every write; keep that so a programming failure the HAL did not
     * report cannot pass as success. */
    if(memcmp((const void *)INFO_PARTITION, info, sizeof(*info)) != 0) {
        return false;
    }
    return (memcmp((const void *)INFO_BACKUP_PARTITION, info, sizeof(*info)) == 0);
}

bool hal_flash_set_goto_iap_mode(bool enable)
{
    flash_info_t info;
    uint8_t value = enable ? GOTO_MODE_STAY_IAP : GOTO_MODE_RUN_AP;

    info_load(&info);
    if(info.goto_mode == value) {
        return true;
    }
    info.goto_mode = value;
    return info_store(&info);
}

bool hal_flash_get_goto_iap_mode(void)
{
    flash_info_t info;

    info_load(&info);
    /* Blank flash reads 0xFF and means "never been set" - boot the AP. */
    return ((info.goto_mode != GOTO_MODE_RUN_AP) && (info.goto_mode != FLASH_ERASED_BYTE));
}

/* ------------------------------------------------------------------------
 * AP partition
 * ------------------------------------------------------------------------ */

bool hal_flash_erase_fw_update_partition(void)
{
#if defined(APP_RUN)
    /* The AP partition is this build's own code. Erasing it would fault on the
     * next instruction fetch, so refuse rather than brick the unit - the SoC
     * gets a clean failure and the update belongs in the MODE=boot build.
     * Note the legacy IAP does not erase here either: Erase Code only arms a
     * flag, and all flash work happens at End Update. */
    return false;
#else
    return flash_erase_sector(APP_PARTITION);
#endif
}

bool hal_flash_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len)
{
#if defined(APP_RUN)
    (void)start_addr;
    (void)data;
    (void)len;
    return false;
#else
    if((start_addr < APP_PARTITION) || (start_addr >= APP_PARTITION_END)) {
        return false;
    }
    /* expressed as a subtraction so a hostile len cannot overflow the sum */
    if((len == 0U) || (len > (APP_PARTITION_END - start_addr))) {
        return false;
    }
    return flash_program(start_addr, data, len);
#endif
}

void hal_flash_read_fw_update_data(uint32_t start_addr, uint8_t *data, uint16_t len)
{
    memcpy(data, (const void *)start_addr, len);
}

/* ------------------------------------------------------------------------
 * Not part of the legacy chimera3 model - it has no staging partition to hand
 * off and no recovery key. Kept so the shared firmware-update interface links
 * the same way on every board.
 * ------------------------------------------------------------------------ */

void hal_flash_set_fw_update_data_size(uint32_t fw_size)
{
    (void)fw_size;
}

void hal_flash_set_fw_update_ready_flag(bool flag)
{
    (void)flag;
}

void hal_flash_set_soc_recovery_flag(bool flag)
{
    (void)flag;
}

bool hal_flash_get_soc_recovery_flag(void)
{
    return false;
}

void hal_flash_clear_soc_recovery_flag(void)
{
}

void hal_flash_init(void)
{
    /* Nothing to reconcile at boot in this model: the IAP owns the update
     * session end to end and clears GoToMode itself before handing over. */
}
