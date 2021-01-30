#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

void serial_write_SB(uint8_t data);
void serial_write_SC(uint8_t data);
uint8_t serial_read_SB(void);
uint8_t serial_read_SC(void);

#endif