#ifndef MID_ERP_FLOW_H__
#define MID_ERP_FLOW_H__

#include <stdbool.h>
#include <stdint.h>
typedef enum {
    ENTER_ERP_READY_STATUS = 0,
    LCB_WAKE_UP_STATUS,
    LCB_ENTER_ERP_STATUS,
    LCB_INIT_STATUS,
    SOC_POWER_OFF_STATUS,
    LEAVE_ERP_READY_STATUS,
} ERP_STATUS;

void mid_set_erp_startup_delay_ms(uint16_t delay_ms);
void mid_set_erp_wakeup_led_blink(bool enable);
uint8_t mid_erp_status_get(void);
void mid_erp_status_set(ERP_STATUS status);
void mid_erp_status_handler(void);

#endif //MID_ERP_FLOW_H__