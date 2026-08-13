#ifndef HAL_FLASH_MAP_H__
#define HAL_FLASH_MAP_H__

#include <stdint.h>

/*
 * chimera3 / STM32F407ZET6 - 512KB internal flash, sector 0~7 only.
 *
 * Shared by both builds: MODE=app links the application, MODE=boot links the
 * IAP, and the two have to agree on every address in here. Keep this header as
 * the single source of truth - nothing else should spell out a flash address.
 *
 * The partition names mirror the legacy chimera3 map (MemoryFlashWriteRead.h)
 * on purpose: the existing field-proven IAP reads _INFOCTL from INFO_PARTITION
 * and jumps to APP_PARTITION, so moving either would break it.
 */

/* Sector base addresses. The device has 8 sectors; 8..11 exist on the 1MB
 * parts only and are deliberately not defined here. */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000)  /*  16K */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000)  /*  16K */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000)  /*  16K */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000)  /*  16K */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000)  /*  64K */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000)  /* 128K */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000)  /* 128K */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000)  /* 128K */
#define ADDR_FLASH_END          ((uint32_t)0x08080000)  /* end of the 512KB device */

#define SIZE_FLASH_SECTOR_16K   ((uint32_t)0x00004000)
#define SIZE_FLASH_SECTOR_64K   ((uint32_t)0x00010000)
#define SIZE_FLASH_SECTOR_128K  ((uint32_t)0x00020000)

/* Partition map */
#define IAP_PARTITION           ADDR_FLASH_SECTOR_0  /* bootloader */
#define IAP_BACKUP_PARTITION    ADDR_FLASH_SECTOR_1
#define INFO_PARTITION          ADDR_FLASH_SECTOR_2  /* _INFOCTL, carries GoToMode */
#define INFO_BACKUP_PARTITION   ADDR_FLASH_SECTOR_3  /* mirror of the above */
#define RESERVED_PARTITION      ADDR_FLASH_SECTOR_4  /* legacy C_LIB_STORE, unused under GCC */
#define APP_PARTITION           ADDR_FLASH_SECTOR_5  /* AP: runs here, and is written here */
#define APP_PARTITION_END       ADDR_FLASH_SECTOR_6  /* exclusive upper bound of the AP */
#define APP_BACKUP_PARTITION    ADDR_FLASH_SECTOR_6
#define APP_TEMP_PARTITION      ADDR_FLASH_SECTOR_7  /* reserved, currently unused */

#endif //HAL_FLASH_MAP_H__
