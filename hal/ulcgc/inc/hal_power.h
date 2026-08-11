#ifndef HAL_POWER_H__
#define HAL_POWER_H__

#define POWER_MODE_ACTIVE   ((uint8_t)0x00)
#define POWER_MODE_ERP      ((uint8_t)0x01)

uint8_t hal_get_power_mode(void);
void hal_set_soc_power_on(void);
void hal_set_soc_power_off(void);
void hal_set_erp_power_off(void);
void hal_set_erp_wakeup_flag(void);
void hal_set_soc_mcu_reset(void);

#endif //HAL_POWER_H__

