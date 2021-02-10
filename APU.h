#ifndef __APU_H__
#define __APU_H__

#include <stdint.h>

void APU_init(void);
void APU_deinit(void);

void APU_clock(void);

uint8_t APU_read_NR10(void);
uint8_t APU_read_NR11(void);
uint8_t APU_read_NR12(void);
uint8_t APU_read_NR13(void);
uint8_t APU_read_NR14(void);
void APU_write_NR10(uint8_t value);
void APU_write_NR11(uint8_t value);
void APU_write_NR12(uint8_t value);
void APU_write_NR13(uint8_t value);
void APU_write_NR14(uint8_t value);

uint8_t APU_read_NR21(void);
uint8_t APU_read_NR22(void);
uint8_t APU_read_NR23(void);
uint8_t APU_read_NR24(void);
void APU_write_NR21(uint8_t value);
void APU_write_NR22(uint8_t value);
void APU_write_NR23(uint8_t value);
void APU_write_NR24(uint8_t value);

uint8_t APU_read_NR30(void);
uint8_t APU_read_NR31(void);
uint8_t APU_read_NR32(void);
uint8_t APU_read_NR33(void);
uint8_t APU_read_NR34(void);
void APU_write_NR30(uint8_t value);
void APU_write_NR31(uint8_t value);
void APU_write_NR32(uint8_t value);
void APU_write_NR33(uint8_t value);
void APU_write_NR34(uint8_t value);

uint8_t APU_read_NR41(void);
uint8_t APU_read_NR42(void);
uint8_t APU_read_NR43(void);
uint8_t APU_read_NR44(void);\
void APU_write_NR41(uint8_t value);
void APU_write_NR42(uint8_t value);
void APU_write_NR43(uint8_t value);
void APU_write_NR44(uint8_t value);

uint8_t APU_read_NR50(void);
uint8_t APU_read_NR51(void);
uint8_t APU_read_NR52(void);
void APU_write_NR50(uint8_t value);
void APU_write_NR51(uint8_t value);
void APU_write_NR52(uint8_t value);

#endif  // __APU_H__