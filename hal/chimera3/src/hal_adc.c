#include "hal_adc.h"

enum hardware_version hw_id = hw_k3;

void hal_adc_init()
{
}

int hal_get_hw_id(void)
{
    return hw_id;
}