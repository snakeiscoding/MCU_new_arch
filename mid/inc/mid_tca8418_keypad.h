#ifndef MID_TCA8418_KEYPAD_H__
#define MID_TCA8418_KEYPAD_H__

#include <stdint.h>

uint8_t mid_tca8418_clear_all_interrupt(void);
void mid_tca8418_keypad_read_handler(void);
void mid_tca8418_reset_keypad_init(void);
void mid_tca8418_keypad_init(void);

#endif //MID_TCA8418_KEYPAD_H__