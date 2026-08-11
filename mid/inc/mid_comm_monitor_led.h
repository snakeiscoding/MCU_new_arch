#ifndef MID_COMM_MONITOR_LED_H__
#define MID_COMM_MONITOR_LED_H__

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    LED_ON_STATUS = 0,
    LED_OFF_STATUS,
    LED_NO_CHANGE_STATUS,
    LED_IDLE_STATUS,
} LED_COMM_STATUS;

typedef enum {
    NORMAL_COMM_STATUS = 0,
    SOC_LCB_COMM_TIMEOUT_STATUS,
    MCU_LCB_COMM_TEST_STATUS,
    SOC_COMM_FAIL_STATUS,
    SOC_LCB_COMM_FAIL_STATUS,
} SOC_LCB_COMM_STATUS;

LED_COMM_STATUS mid_get_soc_mcu_comm_led_status(void);
void mid_set_soc_mcu_comm_timer(void);

LED_COMM_STATUS mid_get_soc_lcb_comm_led_status(void);
LED_COMM_STATUS mid_get_soc_lcb_comm_timeout_status(void);
void mid_set_soc_lcb_comm_timer(void);
void mid_enable_soc_lcb_comm_timer(bool enable);
void mid_set_mcu_lcb_handshake_success(void);
SOC_LCB_COMM_STATUS mid_get_mcu_lcb_handshake_status(void);

#endif //MID_COMM_MONITOR_LED_H__