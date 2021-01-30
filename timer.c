#include "timer.h"
#include "bus.h"

#define TIMER_ENABLE_BIT         0x04
#define TIMER_FREQUENCY_BITS     0x03

uint16_t frequencies[] = { 1024, 16, 64, 256 };

typedef struct Timer Timer;

struct Timer
{
	uint16_t DIV;  // divide register (R/W) - 0xFF04
	uint8_t TIMA;  // timer counter (R/W)   - 0xFF05
	uint8_t TMA;   // timer modulo (R/W)    - 0xFF06
	uint8_t TAC;   // timer control (R/W)   - 0xFF07

	int overflow;
};

Timer timer;

void timer_init(void)
{
	timer.DIV = 0xABCC;
	timer.TIMA = 0x00;
	timer.TMA = 0x00;
	timer.TAC = 0x00;

	timer.overflow = 0;
}

void timer_clock(void)
{
	//uint16_t old_DIV = timer.DIV;
	//timer.DIV += 4;  // timer is clocked every clock cycles (4.194304 MHz) but function called every machine cycle (1.048576 MHz) - DIV is always counting even if timer is disabled

	//if (timer.TAC & TIMER_ENABLE_BIT)  // if timer is enabled increase TIMA
	//{
	//	switch (timer.TAC & TIMER_FREQUENCY_BITS)  // timer frequency
	//	{
	//		case 0:
	//			if (old_DIV & 1 << 9 && !(timer.DIV & 1 << 9))   // increased every 1024 clock cycles / 256 machine cycles
	//				timer.TIMA++;
	//			break;
	//		case 1:
	//			if (old_DIV & 1 << 3 && !(timer.DIV & 1 << 3))   // increased every 16 clock cycles / 4 machine cycles
	//				timer.TIMA++;
	//			break;
	//		case 2:
	//			if (old_DIV & 1 << 5 && !(timer.DIV & 1 << 5))   // increased every 64 clock cycles / 16 machine cycles
	//				timer.TIMA++;
	//			break;
	//		case 3:
	//			if (old_DIV & 1 << 7 && !(timer.DIV & 1 << 7))   // increased every 256 clock cycles / 64 machine cycles
	//				timer.TIMA++;
	//			break;
	//	}
	//	
	//	if (timer.TIMA == 0x00)  // timer overflow
	//		;
	//}

	uint16_t old_DIV = timer.DIV;
	timer.DIV += 4;    // timer is clocked every clock cycles (4.194304 MHz) but function called every machine cycle (1.048576 MHz) - DIV is always counting even if timer is disabled

	if (timer.TAC & TIMER_ENABLE_BIT)  // if timer is enabled increase TIMA
	{
		if (timer.overflow)
		{
			timer.TIMA = timer.TMA;     // reload timer 
			set_int_flag(INT_TIMER);    // set interrupt flag

			timer.overflow = 0;
		}
		else if (old_DIV & frequencies[timer.TAC & TIMER_FREQUENCY_BITS] >> 1 && !(timer.DIV & frequencies[timer.TAC & TIMER_FREQUENCY_BITS] >> 1))
		{
			timer.TIMA++;

			if (timer.TIMA == 0x00)
				timer.overflow = 1;
		}
	}
}

void timer_write_TIMA(uint8_t value)
{
	timer.TIMA = value;
}

void timer_write_TMA(uint8_t value)
{
	timer.TMA = value;
}

void timer_write_DIV(uint8_t data)
{
	(void)data;

	if (timer.TAC & TIMER_ENABLE_BIT && timer.DIV & frequencies[timer.TAC & TIMER_FREQUENCY_BITS] >> 1)
		timer.TIMA++;

	timer.DIV = 0x0000;  // writing to DIV resets the counter
}

void timer_write_TAC(uint8_t value)
{
	if (timer.TAC & TIMER_ENABLE_BIT)     // timer is enabled
		if (!(value & TIMER_ENABLE_BIT))   // disable timer
		{
			if (timer.DIV & frequencies[timer.TAC & TIMER_FREQUENCY_BITS] >> 1)
				timer.TIMA++;
		}
		else                               // enable timer
			if (timer.DIV & frequencies[timer.TAC & TIMER_FREQUENCY_BITS] >> 1 && !(timer.DIV & frequencies[value & TIMER_FREQUENCY_BITS] >> 1))
				timer.TIMA++;

	timer.TAC = value & 0x07;
}

uint8_t timer_read_TIMA(void)
{
	return timer.TIMA;
}

uint8_t timer_read_TMA(void)
{
	return timer.TMA;
}

uint8_t timer_read_DIV(void)
{
	return timer.DIV >> 8;
}

uint8_t timer_read_TAC(void)
{
	return timer.TAC & 0x07;
}
