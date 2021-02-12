#ifndef __BUS_H__
#define __BUS_H__

#include <stdint.h>

#define INT_ENABLE_REG     0xFFFF
#define INT_FLAG_REG       0xFF0F
#define INT_REG_MASK         0x1F


#define OAM_BASE           0xFE00

#define VRAM_ADDRESS_BASE          0x8000
#define VRAM_END           0x9FFF
#define VRAM_SIZE          0x2000



#define ROM_SIZE           0x8000
#define EXTERNAL_RAM_SIZE  0x2000
#define WORK_RAM_SIZE      0x2000

uint8_t bus_read(uint16_t address);
void bus_write(uint16_t address, uint8_t data);

enum Int_Flag { INT_VBLANK = 0x01, INT_LCD_STAT = 0x02, INT_TIMER = 0x04, INT_SERIAL = 0x08, INT_JOYPAD = 0x10 };

void enable_int(enum Int_Flag interrupt);
void disable_int(enum Int_Flag interrupt);
int check_int_enabled(enum Int_Flag interrupt);

void set_int_flag(enum Int_Flag interrupt);
void clear_int_flag(enum Int_Flag interrupt);
int check_int_flag(enum Int_Flag interrupt);

#endif  // __BUS_H__