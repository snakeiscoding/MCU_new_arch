#include "fmc_operation.h"
#include "hal_flash.h"
#include "hal_systick.h"

#define ADDR_FMC_SECTOR_0       ((uint32_t)0x08000000U) /*!< base address of sector 0,  16 kbytes */
#define ADDR_FMC_SECTOR_1       ((uint32_t)0x08004000U) /*!< base address of sector 1,  16 kbytes */
#define ADDR_FMC_SECTOR_2       ((uint32_t)0x08008000U) /*!< base address of sector 2,  16 kbytes */
#define ADDR_FMC_SECTOR_3       ((uint32_t)0x0800C000U) /*!< base address of sector 3,  16 kbytes */
#define ADDR_FMC_SECTOR_4       ((uint32_t)0x08010000U) /*!< base address of sector 4,  64 kbytes */
#define ADDR_FMC_SECTOR_5       ((uint32_t)0x08020000U) /*!< base address of sector 5,  128 kbytes */
#define ADDR_FMC_SECTOR_6       ((uint32_t)0x08040000U) /*!< base address of sector 6,  128 kbytes */
#define ADDR_FMC_SECTOR_7       ((uint32_t)0x08060000U) /*!< base address of sector 7,  128 kbytes */
#define ADDR_FMC_SECTOR_8       ((uint32_t)0x08080000U) /*!< base address of sector 8,  128 kbytes */
#define ADDR_FMC_SECTOR_9       ((uint32_t)0x080A0000U) /*!< base address of sector 9,  128 kbytes */
#define ADDR_FMC_SECTOR_10      ((uint32_t)0x080C0000U) /*!< base address of sector 10, 128 kbytes */
#define ADDR_FMC_SECTOR_11      ((uint32_t)0x080E0000U) /*!< base address of sector 11, 128 kbytes */
#define ADDR_FMC_SECTOR_12      ((uint32_t)0x08100000U) /*!< base address of sector 12, 128 kbytes */
#define ADDR_FMC_SECTOR_13      ((uint32_t)0x08120000U) /*!< base address of sector 13, 128 kbytes */
#define ADDR_FMC_SECTOR_14      ((uint32_t)0x08140000U) /*!< base address of sector 14, 128 kbytes */
#define ADDR_FMC_SECTOR_15      ((uint32_t)0x08160000U) /*!< base address of sector 15, 128 kbytes */
#define ADDR_FMC_SECTOR_16      ((uint32_t)0x08180000U) /*!< base address of sector 16, 128 kbytes */
#define ADDR_FMC_SECTOR_17      ((uint32_t)0x081A0000U) /*!< base address of sector 17, 128 kbytes */
#define ADDR_FMC_SECTOR_18      ((uint32_t)0x081C0000U) /*!< base address of sector 18, 128 kbytes */
#define ADDR_FMC_SECTOR_19      ((uint32_t)0x081E0000U) /*!< base address of sector 19, 128 kbytes */
#define ADDR_FMC_SECTOR_20      ((uint32_t)0x08200000U) /*!< base address of sector 20, 16 kbytes */
#define ADDR_FMC_SECTOR_21      ((uint32_t)0x08204000U) /*!< base address of sector 21, 16 kbytes */
#define ADDR_FMC_SECTOR_22      ((uint32_t)0x08208000U) /*!< base address of sector 22, 16 kbytes */
#define ADDR_FMC_SECTOR_23      ((uint32_t)0x0820C000U) /*!< base address of sector 23, 16 kbytes */
#define ADDR_FMC_SECTOR_24      ((uint32_t)0x08210000U) /*!< base address of sector 24, 64 kbytes */
#define ADDR_FMC_SECTOR_25      ((uint32_t)0x08220000U) /*!< base address of sector 25, 128 kbytes */
#define ADDR_FMC_SECTOR_26      ((uint32_t)0x08240000U) /*!< base address of sector 26, 128 kbytes */
#define ADDR_FMC_SECTOR_27      ((uint32_t)0x08260000U) /*!< base address of sector 27, 128 kbytes */
#define ADDR_FMC_SECTOR_28      ((uint32_t)0x08280000U) /*!< base address of sector 28, 128 kbytes */
#define ADDR_FMC_SECTOR_29      ((uint32_t)0x082A0000U) /*!< base address of sector 29, 128 kbytes */
#define ADDR_FMC_SECTOR_30      ((uint32_t)0x082C0000U) /*!< base address of sector 30, 128 kbytes */
#define ADDR_FMC_SECTOR_31      ((uint32_t)0x082E0000U) /*!< base address of sector 31, 128 kbytes */
#define ADDR_FMC_SECTOR_32      ((uint32_t)0x08300000U) /*!< base address of sector 32, 128 kbytes */
#define ADDR_FMC_SECTOR_33      ((uint32_t)0x08320000U) /*!< base address of sector 33, 128 kbytes */
#define ADDR_FMC_SECTOR_34      ((uint32_t)0x08340000U) /*!< base address of sector 34, 128 kbytes */
#define ADDR_FMC_SECTOR_35      ((uint32_t)0x08360000U) /*!< base address of sector 35, 128 kbytes */
#define ADDR_FMC_SECTOR_36      ((uint32_t)0x08380000U) /*!< base address of sector 36, 128 kbytes */
#define ADDR_FMC_SECTOR_37      ((uint32_t)0x083A0000U) /*!< base address of sector 37, 128 kbytes */
#define ADDR_FMC_SECTOR_38      ((uint32_t)0x083C0000U) /*!< base address of sector 38, 128 kbytes */
#define ADDR_FMC_SECTOR_39      ((uint32_t)0x083E0000U) /*!< base address of sector 39, 128 kbytes */
#define ADDR_FMC_SECTOR_40      ((uint32_t)0x08400000U) /*!< base address of sector 40, 256 kbytes */
#define ADDR_FMC_SECTOR_41      ((uint32_t)0x08440000U) /*!< base address of sector 41, 256 kbytes */
#define ADDR_FMC_SECTOR_42      ((uint32_t)0x08480000U) /*!< base address of sector 42, 256 kbytes */
#define ADDR_FMC_SECTOR_43      ((uint32_t)0x084C0000U) /*!< base address of sector 43, 256 kbytes */
#define ADDR_FMC_SECTOR_44      ((uint32_t)0x08500000U) /*!< base address of sector 44, 256 kbytes */
#define ADDR_FMC_SECTOR_45      ((uint32_t)0x08540000U) /*!< base address of sector 45, 256 kbytes */
#define ADDR_FMC_SECTOR_46      ((uint32_t)0x08580000U) /*!< base address of sector 46, 256 kbytes */
#define ADDR_FMC_SECTOR_47      ((uint32_t)0x085C0000U) /*!< base address of sector 47, 256 kbytes */
#define ADDR_FMC_SECTOR_48      ((uint32_t)0x08600000U) /*!< base address of sector 48, 256 kbytes */
#define ADDR_FMC_SECTOR_49      ((uint32_t)0x08640000U) /*!< base address of sector 49, 256 kbytes */
#define ADDR_FMC_SECTOR_50      ((uint32_t)0x08680000U) /*!< base address of sector 50, 256 kbytes */
#define ADDR_FMC_SECTOR_51      ((uint32_t)0x086C0000U) /*!< base address of sector 51, 256 kbytes */
#define ADDR_FMC_SECTOR_52      ((uint32_t)0x08700000U) /*!< base address of sector 52, 256 kbytes */
#define ADDR_FMC_SECTOR_53      ((uint32_t)0x08740000U) /*!< base address of sector 53, 256 kbytes */

const uint32_t flash_adr_array[] = {
    ADDR_FMC_SECTOR_0, ADDR_FMC_SECTOR_1, ADDR_FMC_SECTOR_2,ADDR_FMC_SECTOR_3,
    ADDR_FMC_SECTOR_4, ADDR_FMC_SECTOR_5, ADDR_FMC_SECTOR_6,ADDR_FMC_SECTOR_7,
    ADDR_FMC_SECTOR_8, ADDR_FMC_SECTOR_9, ADDR_FMC_SECTOR_10,ADDR_FMC_SECTOR_11,
    ADDR_FMC_SECTOR_12, ADDR_FMC_SECTOR_13, ADDR_FMC_SECTOR_14,ADDR_FMC_SECTOR_15,
    ADDR_FMC_SECTOR_16, ADDR_FMC_SECTOR_17, ADDR_FMC_SECTOR_18,ADDR_FMC_SECTOR_19,
    ADDR_FMC_SECTOR_20, ADDR_FMC_SECTOR_21, ADDR_FMC_SECTOR_22,ADDR_FMC_SECTOR_23,
    ADDR_FMC_SECTOR_24, ADDR_FMC_SECTOR_25, ADDR_FMC_SECTOR_26,ADDR_FMC_SECTOR_27,
    ADDR_FMC_SECTOR_28, ADDR_FMC_SECTOR_29, ADDR_FMC_SECTOR_30,ADDR_FMC_SECTOR_31,
    ADDR_FMC_SECTOR_32, ADDR_FMC_SECTOR_33, ADDR_FMC_SECTOR_34,ADDR_FMC_SECTOR_35,
    ADDR_FMC_SECTOR_36, ADDR_FMC_SECTOR_37, ADDR_FMC_SECTOR_38,ADDR_FMC_SECTOR_39,
    ADDR_FMC_SECTOR_40, ADDR_FMC_SECTOR_41};

bool hal_flash_erase_fw_update_partition(void)
{
    for(int i = 22; i < 39; i++) {
        fmc_erase_sector_by_address(flash_adr_array[i]);
    }
    return true;
}

bool hal_flash_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len)
{
    uint16_t i;
    uint16_t temp;
    fmc_state_enum fmc_state = FMC_READY;
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    for(i = 0; i < len / 2; i++) {
        fmc_state = fmc_halfword_program(start_addr, *(uint16_t *)data);
        if(fmc_state != FMC_READY) {
            fmc_lock();
            return false;
        }

        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        data += 2;
        start_addr += 2;
    }
    if(len % 2) {
        temp = *data | 0xff00;
        fmc_state = fmc_halfword_program(start_addr, temp);
    }
    fmc_lock();
    return true;
}

void hal_flash_read_fw_update_data(uint32_t start_addr, uint8_t *data, uint16_t len)
{
    uint32_t addr;
    uint16_t i;
    addr = start_addr;
    for(i = 0; i < len; i++) {
        *data = *(uint8_t *)addr;
        addr = addr + 1;
        data++;
    }
}

#define FW_SIZE_ADDR ((uint32_t) 0x083C0000)
void hal_flash_set_fw_update_data_size(uint32_t fw_size)
{
    uint32_t *p_addr = (uint32_t *)FW_SIZE_ADDR;
    if(*p_addr == fw_size) {
        return;
    }
    fmc_unlock();
    while(FMC_READY != fmc_page_erase((uint32_t)p_addr)) {
        hal_delay_1ms(1);
    }
    while (FMC_READY != fmc_word_program((uint32_t)p_addr, fw_size)) {
        hal_delay_1ms(1);
    }
    fmc_lock();
}

#define FW_READY_FLAG_ADDR ((uint32_t) 0x083D0000)
void hal_flash_set_fw_update_ready_flag(bool flag)
{
    const uint32_t addr = FW_READY_FLAG_ADDR;
    uint32_t value = flag ? 0x99999999 : 0xFFFFFFFF;
    uint32_t addr_data = *((uint32_t *)addr);

    fmc_state_enum erase_state = FMC_BUSY;
    if(addr_data != value) {
        fmc_unlock();
        while(FMC_READY != erase_state) {
            erase_state = fmc_page_erase(addr);
            hal_delay_1ms(1);
        }
        while(FMC_READY != fmc_word_program(addr, value)) {
            hal_delay_1ms(1);
        }
        fmc_lock();
    }
}

bool hal_flash_check_fw_update_partition_flag(void)
{
    uint32_t addr = FW_READY_FLAG_ADDR;
    uint32_t addr_data = *(__IO uint32_t *)(addr);
    if(addr_data == 0x99999999) {
        return true;
    }
    else {
        return false;
    }	
}

#define SOC_RECOVERY_FLAG_ADDR ((uint32_t) 0x083E0000)
void hal_flash_set_soc_recovery_flag(bool flag)
{
    const uint32_t addr = SOC_RECOVERY_FLAG_ADDR;
    uint32_t value = flag ? 0x99999999 : 0xFFFFFFFF;
    uint32_t addr_data = *((uint32_t *)addr);

    fmc_state_enum erase_state = FMC_BUSY;
    if(addr_data != value) {
        fmc_erase_sector_by_address(flash_adr_array[39]);
        fmc_unlock();
        while(FMC_READY != erase_state) {
            erase_state = fmc_page_erase(addr);
            hal_delay_1ms(1);
        }
        while(FMC_READY != fmc_word_program(addr, value)) {
            hal_delay_1ms(1);
        }
        fmc_lock();
    }
}

bool hal_flash_get_soc_recovery_flag(void)
{
    uint32_t addr = SOC_RECOVERY_FLAG_ADDR;
    uint32_t addr_data = *(__IO uint32_t *)(addr);
    if(addr_data == 0x99999999) {
        return true;
    }
    else {
        return false;
    }	
}

void hal_flash_clear_soc_recovery_flag(void)
{
    fmc_erase_sector_by_address(flash_adr_array[39]);
}

void hal_flash_init(void)
{
    if(hal_flash_check_fw_update_partition_flag()) {
        hal_flash_erase_fw_update_partition();
    } 
}