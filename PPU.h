#ifndef __PPU_H__
#define __PPU_H_

#include <stdint.h>

void PPU_init(void);
void PPU_clock(void);

void PPU_write_LCDC(uint8_t value);
void PPU_write_STAT(uint8_t value);
void PPU_write_SCY(uint8_t value);
void PPU_write_SCX(uint8_t value);
void PPU_write_LYC(uint8_t value);
void PPU_write_BGP(uint8_t value);
void PPU_write_OBJP0(uint8_t value);
void PPU_write_OBJP1(uint8_t value);
void PPU_write_WY(uint8_t value);
void PPU_write_WX(uint8_t value);

uint8_t PPU_read_LCDC(void);
uint8_t PPU_read_STAT(void);
uint8_t PPU_read_SCY(void);
uint8_t PPU_read_SCX(void);
uint8_t PPU_read_LY(void);
uint8_t PPU_read_LYC(void);
uint8_t PPU_read_WY(void);
uint8_t PPU_read_WX(void);

#endif  // __PPU_H_