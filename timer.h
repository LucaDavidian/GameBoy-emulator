#if !defined __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

void timer_init(void);
void timer_clock(void); 
void timer_write_TIMA(uint8_t value);
void timer_write_TMA(uint8_t value);
void timer_write_DIV(uint8_t data);
void timer_write_TAC(uint8_t value);
uint8_t timer_read_TIMA(void);
uint8_t timer_read_TMA(void);
uint8_t timer_read_DIV(void);
uint8_t timer_read_TAC(void);

#endif  // __TIMER_H__
