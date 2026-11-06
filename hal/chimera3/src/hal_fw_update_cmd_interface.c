/*
 * hal_fw_update_cmd_interface.c - chimera3 / STM32F407ZET6
 *
 * Overrides the weak hal_* hooks declared in app_soc_cmd_update_func.c and
 * drives hal_flash.c. See hal_flash_map.h for the flash partition map, and
 * the hal_flash.c header for the legacy update sequence this has to
 * interoperate with.
 *
 * The addresses reported through hal_get_fw_update_parameter() are the AP's
 * *execution* addresses. They match the legacy _MEMINF (chimera3io,
 * CommonInternalDriver/Src/MemoryFlashWriteRead.c) field for field, except
 * the NVRAM pair, which is deliberately left at 0 until the provisioning area
 * is migrated - the legacy reported 0x0800A000 / 0x0800BFFF there. (The GD32
 * boards report a separate staging partition instead; that difference lives
 * entirely here.)
 *
 * !! The image format constants below (signature header, CRC16 parameters,
 * !! XOR keys) were carried over from the GD32 boards and have NO counterpart
 * !! on chimera3: the legacy IAP's Program State performs no verification at
 * !! all and unconditionally reports "updated". Nothing has ever validated a
 * !! chimera3 image this way, so confirm with the SoC side that the image is
 * !! produced with the same packer as the GD32 boards before trusting
 * !! hal_get_fw_update_program_state().
 */

#include <string.h>
#include "hal_flash.h"
#include "hal_flash_map.h"
#include "hal_fw_update_cmd_interface.h"
#include "hal_systick.h"
#include "mid_log.h"
#include "stm32f4xx.h"

#define HAL_AP_START_ADDRESS        APP_PARTITION                       /* 0x08020000 */
#define HAL_AP_LIMIT                APP_PARTITION_END                   /* 0x08040000 */
#define HAL_AP_END_ADDRESS          (APP_PARTITION_END - 1)             /* 0x0803FFFF */
#define HAL_PAGE_SIZE               ((uint32_t)0x00000200)              /* from the legacy _MEMINF */
#define HAL_MAX_PAGES_PER_PACKET    ((uint32_t)0x00000001)
#define HAL_NVRAM_START_ADDRESS     ((uint32_t)0x00000000)              /* not migrated yet */
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

/* Erasing the 128K AP sector blocks for on the order of a second. It is a
 * single shot on purpose: retrying an erase that reported an error does not
 * help, and the SoC gets a clean failure instead of a stalled command.
 * In the MODE=app build hal_flash_erase_fw_update_partition() always refuses,
 * because the AP partition is the running image.
 * The legacy IAP does no flash work on this command at all - it only arms an
 * internal flag - so a MODE=boot build is free to erase here instead, as long
 * as End Update still backs the old image up before committing. */
SOC_CMD_UPDATE_STATUS hal_erase_fw_update_partition(void)
{
    if(!hal_flash_erase_fw_update_partition()) {
        mid_log_print(LOG_ERROR, "fw update erase AP partition failed\r\n");
        return UPDATE_FAIL;
    }
    total_write_fw_len = 0;
    return UPDATE_SUCCESS;
}

/* The two bounds deliberately use different constants.
 *
 * The start address is held to HAL_AP_END_ADDRESS (0x0803FFFF, the last valid
 * byte): a write that begins on the very last byte can only ever carry one
 * byte and is meaningless for a page-based protocol, so it is refused outright.
 * That costs nothing - the last byte is still reachable as the tail of a write
 * that starts earlier, and the final 512-byte page starts at 0x0803FE00, well
 * inside the bound.
 *
 * The length, however, has to be measured against HAL_AP_LIMIT (0x08040000,
 * the exclusive end). Using HAL_AP_END_ADDRESS there loses a byte and rejects
 * that final full page: 0x0803FFFF - 0x0803FE00 is 511, one short of the 512
 * the page needs. This also keeps the length check in step with the one in
 * hal_flash_write_fw_update_data(). */
SOC_CMD_UPDATE_STATUS hal_fw_validate_start_addr(uint32_t start_addr, uint32_t len)
{
    if(start_addr < HAL_AP_START_ADDRESS || start_addr >= HAL_AP_END_ADDRESS) {
        return UPDATE_FAIL;
    }
    /* expressed as a subtraction so a hostile len cannot overflow the sum */
    if(len == 0 || len > (HAL_AP_LIMIT - start_addr)) {
        return UPDATE_FAIL;
    }
    return UPDATE_SUCCESS;
}

/* Programs the AP partition in place, which is NOT what the legacy IAP does -
 * it stages each page into SRAM1 and only commits at End Update. Writing in
 * place is simpler but gives up the legacy's implicit rollback, so a MODE=boot
 * build using this path must back the old image up itself.
 *
 * Not retried: a partial write leaves those cells programmed, so a second pass
 * over the same address would program non-erased flash and raise PGSERR every
 * time. Report the failure and let the SoC restart the update. */
SOC_CMD_UPDATE_STATUS hal_write_fw_update_data(uint32_t start_addr, uint8_t *data, uint32_t len)
{
    if(hal_fw_validate_start_addr(start_addr, len) == UPDATE_FAIL) {
        mid_log_print(LOG_ERROR, "fw write bad range addr : 0x%08x, len : %d\r\n", start_addr, len);
        return UPDATE_FAIL;
    }

    if(!hal_flash_write_fw_update_data(start_addr, &data[0], len)) {
        mid_log_print(LOG_ERROR, "fw write failed addr : 0x%08x, len : %d\r\n", start_addr, len);
        return UPDATE_FAIL;
    }

    total_write_fw_len += len;
    mid_log_print(LOG_INFO, "fw write addr : 0x%08x, len : %d, total len : %d\r\n", start_addr, len, total_write_fw_len);
    return UPDATE_SUCCESS;
}

#define FW_UPDATE_PASSWORD      "johnson"
#define FW_UPDATE_PASSWORD_LEN  ((uint16_t)(sizeof(FW_UPDATE_PASSWORD) - 1))

bool hal_verify_fw_update_password(uint8_t *data, uint16_t len)
{
    if(len != FW_UPDATE_PASSWORD_LEN) {
        return false;
    }
    return (memcmp(data, FW_UPDATE_PASSWORD, FW_UPDATE_PASSWORD_LEN) == 0);
}
/* Runs in the AP when the host opens a session: latch GoToMode so the next
 * reset lands in the IAP, which hosts the rest of the update. Mirrors the
 * legacy SocUart.c StartUpdate(), which latches the same byte in the same
 * INFO block; the only difference is how it leaves the AP - the legacy did
 * HAL_DeInit() + a direct MemoryFuncJump(JumpToIap), this resets instead and
 * lets the IAP re-read GoToMode from its own reset path. */
void hal_start_fw_update(void)
{
    if(!hal_flash_set_goto_iap_mode(true)) {
        mid_log_print(LOG_ERROR, "fw update failed to set GoToMode\r\n");
        return;   /* stay in the AP rather than reset into a latch that is not set */
    }
    hal_delay_1ms(2);
    HAL_NVIC_SystemReset();
}
/* Runs in the IAP at the end of a session: drop the latch so the next reset
 * boots the freshly written AP instead of coming back into the bootloader.
 * Only the latch is handled here. The legacy IAP does considerably more at
 * End Update - it backs the running AP up to APP_BACKUP_PARTITION and commits
 * the staged SRAM1 image into APP_PARTITION before clearing GoToMode - so this
 * is not yet a complete stand-in for it. */
void hal_end_fw_update(void)
{
    if(!hal_flash_set_goto_iap_mode(false)) {
        mid_log_print(LOG_ERROR, "fw update failed to clear GoToMode\r\n");
    }
    hal_delay_1ms(2);
    HAL_NVIC_SystemReset();
}

#define CRC16_CMD_LEN ((uint8_t)10)
const uint8_t crc16_cmd_header[4] = {0x56, 0x0F, 0x04, 0x00};
static uint16_t crc16_reflect(uint16_t data, uint8_t bits)
{
    uint8_t bit;
    uint32_t reflection = 0x00000000;
    for(bit = 0; bit < bits; bit++) {
        if ((data & 0x01) != 0) {
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
    if (reflectOut != 0) {
        crc16 = (uint16_t)((crc16_reflect((uint16_t)crc16, 16) ^ xorOut) & mask);
    }

    return crc16;
}

uint32_t crc32_cmd_checksum = 0;
uint32_t crc16_cmd_start_addr = 0;
SOC_CMD_UPDATE_STATUS hal_fw_update_verify_crc16_checksum(void)
{
    uint8_t crc16_cmd[CRC16_CMD_LEN];

    if(total_write_fw_len < CRC16_CMD_LEN) {
        return UPDATE_FAIL;
    }

    for(uint32_t start_addr = (HAL_AP_START_ADDRESS + total_write_fw_len - CRC16_CMD_LEN); start_addr >= HAL_AP_START_ADDRESS; start_addr--) {
        hal_flash_read_fw_update_data(start_addr, &crc16_cmd[0], 10);

        if(memcmp(crc16_cmd, crc16_cmd_header, 4) == 0) {
            uint16_t crc16_cmd_checksum = (crc16_cmd[9] << 8) + crc16_cmd[8];
            uint16_t crc16_checksum = hal_fw_update_get_crc16_checksum(&crc16_cmd[0], 8, 1, 1, 0x1021, 0xFFFF, 0x0000, 0x8000, 0xFFFF);
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

    if(crc16_cmd_start_addr < (HAL_AP_START_ADDRESS + SIGNATURE_LEN)) {
        return UPDATE_FAIL;
    }

    hal_flash_read_fw_update_data(crc16_cmd_start_addr - SIGNATURE_LEN, &signature_data[0], 38);
    if(memcmp(signature_data, signature_header, 12) == 0) {
        return UPDATE_SUCCESS;
    }
    return UPDATE_FAIL;
}

/* The packer XORs every byte of the CRC32 with the low byte of each key only,
 * so that is the protocol: 0xBE, then 0x61 (net 0xDF per byte). Confirmed on
 * hardware - a packed image sent through the SoC update flow passes Program
 * State - and ulcgc, vortex2 and vortex_lite2 decode the same way.
 * Do not widen this to the full 32-bit values below; the packer does not use them,
 * and every update would fail the CRC32 check.
 *   PUBLIC_KEY  is the low byte of 0x332818BE
 *   PRIVATE_KEY is the low byte of 0x0FB4C161 */
#define PRIVATE_KEY ((uint8_t) 0x61)
#define PUBLIC_KEY  ((uint8_t) 0xBE)

static void hal_xor_encrypt_decrypt(uint8_t key, uint8_t *data, size_t len)
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
            if (crc32 & 1) {
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
    for (size_t i = 0; i < len; i++) {
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
    /* No staging hand-off to record here: this path assumes the image is
     * already in the AP partition, so a pass simply tells the SoC it may end
     * the session, and a failure means the AP is now damaged and the host has
     * to run the update again before resetting.
     *
     * That assumption only holds for an in-place writer. Under the legacy
     * model the image is still sitting in SRAM1 at this point and the AP
     * partition has not been touched yet, so these checks would be reading the
     * *old* image. A MODE=boot build has to settle which of the two write
     * models it uses before this function means anything. */
    return UPDATE_SUCCESS;
}
