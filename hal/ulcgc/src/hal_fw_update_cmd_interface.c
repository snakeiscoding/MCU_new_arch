#include <string.h>
#include "hal_flash.h"
#include "hal_fw_update_cmd_interface.h"
#include "hal_power.h"
#include "hal_systick.h"
#include "mid_log.h"

#define HAL_AP_START_ADDRESS        ((uint32_t)0x08208000)
#define HAL_AP_END_ADDRESS          ((uint32_t)0x083C0000 - 0x00000200)
#define HAL_PAGE_SIZE               ((uint32_t)0x00000200)
#define HAL_MAX_PAGES_PER_PACKET    ((uint32_t)0x00000001)
#define HAL_NVRAM_START_ADDRESS     ((uint32_t)0x00000000)
#define HAL_NVRAM_END_ADDRESS       ((uint32_t)0x00000000)

uint32_t hal_get_fw_update_parameter(SOC_CMD_UPDATE_PARAMETER_ID id)
{
    switch((SOC_CMD_UPDATE_PARAMETER_ID)id) {  
        case AP_START_ADDRESS:
            return HAL_AP_START_ADDRESS;
        break;        
        case AP_END_ADDRESS:
            return HAL_AP_END_ADDRESS;
        break;
        case PAGE_SIZE:
            return HAL_PAGE_SIZE;
        break;
        case MAX_PAGES_PER_PACKET:
            return HAL_MAX_PAGES_PER_PACKET;
        break;
        case NVRAM_START_ADDRESS:
            return HAL_NVRAM_START_ADDRESS;
        break;
        case NVRAM_END_ADDRESS:
            return HAL_NVRAM_END_ADDRESS;
        break;
    }
    return 0;
}

uint32_t total_write_fw_len = 0;
SOC_CMD_UPDATE_STATUS hal_erase_fw_update_partition(void)
{
    uint32_t erase_timeout = 0;
    while(!hal_flash_erase_fw_update_partition()) {
        hal_delay_1ms(1);
        erase_timeout++;
        if(erase_timeout > 1000) {
			return UPDATE_FAIL;
		}
    }
    total_write_fw_len = 0;
    return UPDATE_SUCCESS;
}

SOC_CMD_UPDATE_STATUS hal_fw_validate_start_addr(uint32_t start_addr, uint32_t len)
{
    if(start_addr < HAL_AP_START_ADDRESS ||
    start_addr >= HAL_AP_END_ADDRESS ||
    len == 0 ||
    len >= HAL_AP_END_ADDRESS - start_addr) {
        return UPDATE_FAIL;
    }
    return UPDATE_SUCCESS;
}

SOC_CMD_UPDATE_STATUS hal_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len)
{
    if(hal_fw_validate_start_addr(start_addr, len) == UPDATE_FAIL) {
        return UPDATE_FAIL;
    }

    uint32_t write_timeout = 0;
    while(!hal_flash_write_fw_update_data(start_addr, &data[0], len)) {
        hal_delay_1ms(1);
        write_timeout++;
        if(write_timeout > 500) {
			return UPDATE_FAIL;
		}
    }
    total_write_fw_len += len;
    mid_log_print(LOG_INFO, "fw write addr : 0x%08x, len : %d, total len : %d\r\n", start_addr, len, total_write_fw_len);
    return UPDATE_SUCCESS;
}

/* 
 * Auto-reboot after update was removed in V1.7.
 * SoC need to reset MCU via reboot command (0x0006) or re-powering to complete the update.
 */
void hal_end_fw_update(void)
{
    mid_log_print(LOG_INFO, "End of firmware update\r\n");
    hal_delay_1ms(2);
}

#define CRC16_CMD_LEN ((uint8_t)10)
const uint8_t crc16_cmd_header[4] = {0x56, 0x0F, 0x04, 0x00};
static uint16_t crc16_reflect(uint16_t data, uint8_t bits)
{
    uint8_t bit;
    uint32_t reflection = 0x00000000;
    for(bit = 0; bit < bits; bit++) {
        if((data & 0x01) != 0) {
            reflection |= (uint32_t)(1 << ((bits - 1) - bit));
        }
        data = (uint16_t)(data >> 1);
    }

    return reflection;
}

uint16_t hal_fw_update_get_crc16_checksum(uint8_t *data, uint16_t len, uint8_t reflectIn, uint8_t reflectOut, uint16_t polynomial, uint16_t xorIn, uint16_t xorOut, uint16_t msbMask, uint16_t mask)
{
    uint16_t crc16 = xorIn;
    uint16_t i, j;
    uint8_t c;
    uint32_t bit;

    if(len == 0) {
        return crc16;
    }

    for(i = 0; i < len; i++) {
        c = data[i];
        if(reflectIn != 0) {
			c = (uint8_t)crc16_reflect(c, 8);
        }   
		j = 0x80;
		while(j > 0) {
            bit = (uint32_t)(crc16 & msbMask);
            crc16 <<= 1;
            if((c & j) != 0) {
                bit = (uint32_t)(bit ^ msbMask);
			}
            if(bit != 0) {
                crc16 ^= polynomial;
            }
            j >>= 1;
        }
    }
    if(reflectOut != 0) {
        crc16 = (uint16_t)((crc16_reflect((uint16_t)crc16, 16) ^ xorOut) & mask);
    }
		
	return crc16;
}

uint32_t crc32_cmd_checksum = 0;
uint32_t crc16_cmd_start_addr = 0;
SOC_CMD_UPDATE_STATUS hal_fw_update_verify_crc16_checksum(void)
{
    uint8_t crc16_cmd[CRC16_CMD_LEN];
    for(uint32_t start_addr = (HAL_AP_START_ADDRESS + total_write_fw_len - CRC16_CMD_LEN); start_addr >= HAL_AP_START_ADDRESS; start_addr--) {
        hal_flash_read_fw_update_data(start_addr, &crc16_cmd[0], 10);

        if(memcmp(crc16_cmd, crc16_cmd_header, 4) == 0) {
            uint16_t crc16_cmd_checksum = (crc16_cmd[9] << 8) + crc16_cmd[8];
            uint16_t crc16_checksum = hal_fw_update_get_crc16_checksum(&crc16_cmd[0], 8, 1, 1, 0x1021, 0xFFFF, 0x0000, 0x8000, 0xFFFF);
            //mid_log_print(LOG_INFO, "crc16 cmd checksum 0x%04x\r\n", crc16_cmd_checksum);
            //mid_log_print(LOG_INFO, "crc16 checksum 0x%04x\r\n", crc16_checksum);
            if(crc16_checksum == crc16_cmd_checksum) {
                crc16_cmd_start_addr = start_addr;
                crc32_cmd_checksum = (crc16_cmd[7] << 24) + (crc16_cmd[6] << 16) + (crc16_cmd[5] << 8) + crc16_cmd[4];
                return UPDATE_SUCCESS;
            }
        }
    }
    return UPDATE_FAIL;
}

#define SIGNATURE_LEN ((uint8_t) 38)
const uint8_t signature_header[12] = "info;K2_JHT;";
SOC_CMD_UPDATE_STATUS hal_fw_update_verify_signature(void)
{
    uint8_t signature_data[SIGNATURE_LEN];
    hal_flash_read_fw_update_data(crc16_cmd_start_addr - SIGNATURE_LEN, &signature_data[0], 38);
    if(memcmp(signature_data, signature_header, 12) == 0) {
        //mid_log_print(LOG_INFO, "find the signature %s\r\n", signature_data);
        return UPDATE_SUCCESS;
    }
    return UPDATE_FAIL;
}

#define PRIVATE_KEY ((uint32_t) 0XFB4C161)
#define PUBLIC_KEY  ((uint32_t) 0X332818BE)

static void hal_xor_encrypt_decrypt(uint32_t key, uint8_t *data, size_t len)
{
    for(size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

#define CRC32_POLYNOMIAL ((uint32_t) 0xEDB88320)
static uint32_t crc32_table[256];
uint32_t crc32_checksum = 0xFFFFFFFF;
static void hal_crc32_table_gen(uint32_t table[256])
{
    for(uint32_t i = 0; i < 256; i++) {
        uint32_t crc32 = i;
        for(int j = 0; j < 8; j++) {
            if(crc32 & 1) {
                crc32 = (crc32 >> 1) ^ CRC32_POLYNOMIAL;
            }
            else {
                crc32 >>= 1;
            }
        }
        table[i] = crc32;
    }
}

void hal_crc32_table_reset(void)
{
	crc32_checksum = 0xFFFFFFFF;
	hal_crc32_table_gen(crc32_table);
}

static void hal_crc32_calculate(uint8_t *data, uint16_t len)
{
    for(size_t i = 0; i < len; i++) {
        crc32_checksum = (crc32_checksum >> 8) ^ crc32_table[(crc32_checksum & 0xFF) ^ data[i]];
    }
}

uint32_t hal_fw_update_get_crc32_checksum()
{
    crc32_checksum ^= 0xFFFFFFFF;
    return crc32_checksum;
}

SOC_CMD_UPDATE_STATUS hal_fw_update_verify_crc32_checksum(void)
{
    hal_xor_encrypt_decrypt(PUBLIC_KEY, (uint8_t *)&crc32_cmd_checksum, sizeof(crc32_cmd_checksum));
	hal_xor_encrypt_decrypt(PRIVATE_KEY, (uint8_t *)&crc32_cmd_checksum, sizeof(crc32_cmd_checksum));

    hal_crc32_table_reset();
    for(uint32_t start_addr = HAL_AP_START_ADDRESS; start_addr < crc16_cmd_start_addr; start_addr++) {
        uint8_t read_data = 0;
        hal_flash_read_fw_update_data(start_addr, &read_data, 1);
        hal_crc32_calculate(&read_data, 1);
	}

    if(crc32_cmd_checksum == hal_fw_update_get_crc32_checksum()) {
        //mid_log_print(LOG_INFO, "crc32 cmd checksum 0x%08X\r\n", crc32_cmd_checksum);
        //mid_log_print(LOG_INFO, "crc32 checksum 0x%08X\r\n", hal_fw_update_get_crc32_checksum());
        return UPDATE_SUCCESS;
    }


    return UPDATE_FAIL;
}

SOC_CMD_UPDATE_STATUS hal_get_fw_update_program_state(void)
{
    if(hal_fw_update_verify_crc16_checksum() == UPDATE_FAIL) {
        mid_log_print(LOG_ERROR, "fw update verify crc16 checksum error\r\n");
        return UPDATE_FAIL;
    }
    if(hal_fw_update_verify_signature() == UPDATE_FAIL) {
        mid_log_print(LOG_ERROR, "fw update verify signature error\r\n");
        return UPDATE_FAIL;
    }
    if(hal_fw_update_verify_crc32_checksum() == UPDATE_FAIL) {
        mid_log_print(LOG_ERROR, "fw update verify crc32 checksum erro\r\n");
        return UPDATE_FAIL;
    }
    //mid_log_print(LOG_INFO, "set fw update data size 0x%08X\r\n", crc16_cmd_start_addr - HAL_AP_START_ADDRESS - SIGNATURE_LEN);
    hal_flash_set_fw_update_data_size(crc16_cmd_start_addr - HAL_AP_START_ADDRESS - SIGNATURE_LEN);
    hal_flash_set_fw_update_ready_flag(true);
    return UPDATE_SUCCESS;
}