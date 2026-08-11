#ifndef HAL_UART_H__
#define HAL_UART_H__
#include <stdint.h>

enum UART_ID {
    UART_ID_0,
    UART_ID_1,
    UART_ID_2,
    UART_ID_3,
};

void hal_uart_init();
void hal_uart_send(enum UART_ID uart_id, uint8_t *data, uint8_t len);
#endif //HAL_UART_H__