#include "mid_csafe_cab.h"

__attribute__((weak)) void hal_send_soc_cmd_to_csafe_cab(uint8_t *data, uint16_t len) {}
void mid_send_soc_cmd_to_csafe_cab(uint8_t *data, uint16_t len)
{
    hal_send_soc_cmd_to_csafe_cab(data, len);
}