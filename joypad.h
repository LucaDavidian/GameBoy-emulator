#ifndef __JOYPAD_H__
#define __JOYPAD_H__

#include <stdint.h>

uint8_t joypad_read(void);
void joypad_write(uint8_t data);

#endif  // __JOYPAD_H__