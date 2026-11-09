/*!
    \file    fmc_operation.c
    \brief   flash program, erase

    \version 2024-12-20, V1.2.0, firmware for GD32F5xx
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "fmc_operation.h"
#include "gd32f5xx.h"

/*!
    \brief      get the sector number, size and range of the given address
    \param[in]  address: The flash address
    \param[out] none
    \retval     fmc_sector_info_struct: The information of a sector
*/
fmc_sector_info_struct fmc_sector_info_get(uint32_t addr)
{
    fmc_sector_info_struct sector_info;
    uint32_t temp = 0x00000000U;
    if((FMC_START_ADDRESS <= addr) && (FMC_END_ADDRESS >= addr)) {
        if((FMC_BANK1_START_ADDRESS > addr)) {
            /* bank0 area */
            temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_16KB;
            if(4U > temp) {
                sector_info.sector_name = (uint32_t)temp;
                sector_info.sector_num = CTL_SN(temp);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if(8U > temp) {
                sector_info.sector_name = 4;
                sector_info.sector_num = CTL_SN(4);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_64KB * 1);;
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_64KB - 1;
            } else {
                temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 4);
                sector_info.sector_num = CTL_SN(temp + 4);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
        } else if((FMC_BANK1EX_START_ADDRESS > addr)) {
            /* bank1 area */
            temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_16KB;
            if(4U > temp) {
                sector_info.sector_name = (uint32_t)(temp + 20);
                sector_info.sector_num = CTL_SN(temp + 20);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if(8U > temp) {
                sector_info.sector_name = 24;
                sector_info.sector_num = CTL_SN(24);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_64KB * 1);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_64KB - 1;
            } else {
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 24);
                if(32 > (temp + 24)) {
                    sector_info.sector_num = CTL_SN(temp + 24);
                } else {
                    sector_info.sector_num = SN_5 | CTL_SN(temp + 24);
                }
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
        } else {
            /* bank1_ex area */
            temp = (addr - FMC_BANK1EX_START_ADDRESS) / SIZE_256KB;
            sector_info.sector_name = (uint32_t)(temp + 40);
            sector_info.sector_num = SN_5 | CTL_SN(temp + 40);
            sector_info.sector_size = SIZE_256KB;
            sector_info.sector_start_addr = FMC_BANK1EX_START_ADDRESS + (SIZE_256KB * temp);
            sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_256KB - 1;
        }
    } else {
        /* invalid address */
        sector_info.sector_name = FMC_WRONG_SECTOR_NAME;
        sector_info.sector_num = FMC_WRONG_SECTOR_NUM;
        sector_info.sector_size = FMC_INVALID_SIZE;
        sector_info.sector_start_addr = FMC_INVALID_ADDR;
        sector_info.sector_end_addr = FMC_INVALID_ADDR;
    }
    return sector_info;
}

/*!
    \brief      get the sector number by a given sector name
    \param[in]  address: a given sector name
    \param[out] none
    \retval     uint32_t: sector number
*/
uint32_t sector_name_to_number(uint32_t sector_name)
{
    if(53 >= sector_name) {
        if(32 > sector_name) {
            return CTL_SN(sector_name);
        } else {
            return SN_5 | CTL_SN(sector_name);
        }
    } else {
        while(1) {
        }
    }
}

/*!
    \brief      erases the sector of a given address
    \param[in]  address: a given address
    \param[out] none
    \retval     none
*/
void fmc_erase_sector_by_address(uint32_t address)
{
    fmc_sector_info_struct sector_info;
    // printf("\r\nFMC erase operation:\r\n");
    /* get information about the sector in which the specified address is located */
    sector_info = fmc_sector_info_get(address);
    if(FMC_WRONG_SECTOR_NAME == sector_info.sector_name) {
        printf("\r\nWrong address!\r\n");
        while(1) {
        }
    } else {
        // printf("\r\nErase start ......\r\n");
        /* unlock the flash program erase controller */
        fmc_unlock();
        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);
        /* wait the erase operation complete*/
        if(FMC_READY != fmc_sector_erase(sector_info.sector_num)) {
            while(1) {
            }
        }
        /* lock the flash program erase controller */
        fmc_lock();
        // printf("Address 0x%08X is located in the : SECTOR_NUMBER_%d !\r\n", address, sector_info.sector_name);
        // printf("Sector range: 0x%08X to 0x%08X\r\n", sector_info.sector_start_addr, sector_info.sector_end_addr);
        // printf("Sector size: %d KB\r\n", (sector_info.sector_size / 1024));
        // printf("Erase success!\r\n");
    }
}

/*!
    \brief      write 64 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_64: data pointer
    \param[out] none
    \retval     none
    \note       validate address/length (e.g. hal_fw_validate_start_addr()) before calling
*/
void fmc_write_64bit_data(uint32_t address, uint16_t length, uint64_t *data_64)
{
    if(length == 0) return;

    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    printf("\r\nFMC double word programe operation:\r\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 8 * length - 1);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++) {
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)) {
            while(1) {
            }
        }
    }

    /* write data_64 to the corresponding address */
    for(i = 0; i < length; i++) {
        if(FMC_READY == fmc_doubleword_program(address, data_64[i])) {
            address = address + 8;
        } else {
            while(1) {
            }
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    printf("Write complete!\r\n");
}

/*!
    \brief      read 64 bit length data from a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_64: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_64bit_data(uint32_t address, uint16_t length, uint64_t *data_64)
{
    uint8_t i;
    printf("Read data from 0x%08lX\r\n", address);
    for(i = 0; i < length; i++) {
        data_64[i] = *(__IO uint64_t *)address;
        printf("0x%16llX  ", data_64[i]);
        address = address + 8;
    }
    printf("\r\nRead end\r\n");
}

/*!
    \brief      write 32 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
    \note       validate address/length (e.g. hal_fw_validate_start_addr()) before calling
*/
void fmc_write_32bit_data(uint32_t address, uint16_t length, uint32_t *data_32)
{
    if(length == 0) return;

    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    printf("\r\nFMC word programe operation:\r\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 4 * length - 1);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++) {
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)) {
            while(1) {
            }
        }
    }

    /* write data_32 to the corresponding address */
    for(i = 0; i < length; i++) {
        if(FMC_READY == fmc_word_program(address, data_32[i])) {
            address = address + 4;
        } else {
            while(1) {
            }
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    printf("Write complete!\r\n");
}

/*!
    \brief      read 32 bit length data from a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_32bit_data(uint32_t address, uint16_t length, uint32_t *data_32)
{
    uint8_t i;
    printf("Read data from 0x%08lX\r\n", address);
    for(i = 0; i < length; i++) {
        data_32[i] = *(__IO uint32_t *)address;
        printf("0x%08lX  ", data_32[i]);
        address = address + 4;
    }
    printf("\r\nRead end\r\n");
}

/*!
    \brief      write 16 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_16: data pointer
    \param[out] none
    \retval     none
    \note       validate address/length (e.g. hal_fw_validate_start_addr()) before calling
*/
void fmc_write_16bit_data(uint32_t address, uint16_t length, uint16_t *data_16)
{
    if(length == 0) return;

    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    printf("\r\nFMC half_word program operation:\r\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 2 * length - 1);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++) {
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)) {
            while(1) {
            }
        }
    }

    /* write data_16 to the corresponding address */
    for(i = 0; i < length; i++) {
        if(FMC_READY == fmc_halfword_program(address, data_16[i])) {
            address = address + 2;
        } else {
            while(1) {
            }
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    printf("Write complete!\r\n");
}

/*!
    \brief      read 16 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_16: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_16bit_data(uint32_t address, uint16_t length, uint16_t *data_16)
{
    uint8_t i;
    printf("Read data from 0x%04lX\r\n", address);
    for(i = 0; i < length; i++) {
        data_16[i] = *(__IO uint16_t *)address;
        printf("0x%04X  ", data_16[i]);
        address = address + 2;
    }
    printf("\r\nRead end\r\n");
}

/*!
    \brief      write 8 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_8: data pointer
    \param[out] none
    \retval     none
    \note       validate address/length (e.g. hal_fw_validate_start_addr()) before calling
*/
void fmc_write_8bit_data(uint32_t address, uint16_t length, uint8_t *data_8)
{
    if(length == 0) return;

    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num, i;

    printf("\r\nFMC byte program operation:\r\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + length - 1);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++) {
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)) {
            while(1) {
            }
        }
    }

    /* write data_8 to the corresponding address */
    for(i = 0; i < length; i++) {
        if(FMC_READY == fmc_byte_program(address, data_8[i])) {
            address++;
        } else {
            while(1) {
            }
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    printf("Write complete!\r\n");
}

/*!
    \brief      read 8 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x0877FFFF)
    \param[in]  length: data length
    \param[in]  data_8: data pointer
    \param[out] none
    \retval     none
*/
void fmc_read_8bit_data(uint32_t address, uint16_t length, uint8_t *data_8)
{
    uint8_t i;
    printf("Read data from 0x%02lX\r\n", address);
    for(i = 0; i < length; i++) {
        data_8[i] = *(__IO int8_t *)address;
        printf("0x%02X  ", data_8[i]);
        address++;
    }
    printf("\r\nRead end\r\n");
}

