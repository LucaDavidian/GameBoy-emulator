#include "PPU.h"
#include "SDL2/SDL.h"
#include "bus.h"
#include <stdint.h>

#define PPU_MODE_BITS  0x03
#define PPU_MODE0      0x00
#define PPU_MODE1      0x01
#define PPU_MODE2      0x10
#define PPU_MODE3      0x11

typedef struct PPU PPU;

struct PPU
{
	uint8_t LCDC;  // LCD control (R/W)           - 0xFF40
	union
	{
		struct
		{
			uint8_t mode_flag : 2;
			uint8_t coincidence_flag : 1;
			uint8_t mode0_HBLANK_interrupt : 1;
			uint8_t mode1_VBLANK_interrupt: 1;
			uint8_t mode2_OAM_interrupt : 1;
			uint8_t LYC_interrupt : 1;
			uint8_t : 1;
		} bits;
		uint8_t reg;  // LCD status (R/W)         - 0xFF41
	} STAT;
	uint8_t SCY;   // scroll Y (R/W)              - 0xFF42
	uint8_t SCX;   // scroll X (R/W)              - 0xFF43
	uint8_t LY;    // current scanline (R)        - 0xFF44
	uint8_t LYC;   // LYC compare (R/W)           - 0xFF45
	uint8_t BGP;   // background palette data (W) - 0xFF47
	uint8_t OBJP0; // sprite palette data 0 (W)   - 0xFF48
	uint8_t OBJP1; // sprite palette data 1 (W)   - 0xFF49
	uint8_t WY;    // window y-coordinate (R/W)   - 0xFF4A
	uint8_t WX;    // window x-coordinate (R/W)   - 0xFF4B

	uint8_t scanline;
	uint16_t clocks;   // 4.194304 MHz

	uint16_t pixel_FIFO_high;
	uint16_t pixel_FIFO_low;
};

PPU ppu;

void PPU_init(void)
{
	ppu.clocks = 0;
	ppu.scanline = 0;

	ppu.pixel_FIFO_high = 0;
	ppu.pixel_FIFO_low = 0;
}

void PPU_clock(void)  
{
	if (ppu.scanline >= 0 && ppu.scanline <= 143)          // active display
	{
		if (ppu.clocks >= 0 && ppu.clocks <= 80)           // mode 2 - OAM search
		{
			ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~PPU_MODE_BITS | PPU_MODE2;

			if (check_int_enabled(INT_LCD_STAT))
				set_int_flag(INT_LCD_STAT);
		}
		else if (ppu.clocks >= 81)                         // mode 3 - pixel tranfer
		{
			ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~PPU_MODE_BITS | PPU_MODE3;

			if (check_int_enabled(INT_LCD_STAT))
				set_int_flag(INT_LCD_STAT);
		}
		else                                               // mode 0 - HBLANK
		{
			ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~PPU_MODE_BITS | PPU_MODE0;

			if (check_int_enabled(INT_LCD_STAT))
				set_int_flag(INT_LCD_STAT);
		}
	}
	else if (ppu.scanline >= 144 && ppu.scanline <= 154)   // mode 1 - VBLANK
	{
		ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~PPU_MODE_BITS | PPU_MODE1;

		if (check_int_enabled(INT_VBLANK))
			set_int_flag(INT_VBLANK);
	}


	ppu.clocks += 4;   // every time this function is called (@ 1.048576 MHz) the pixel clock (@ 4.194304 MHz) makes 4 cycles

	if (ppu.clocks > 375)  // each scanline is 376 clocks
	{
		ppu.scanline++;
		ppu.clocks = 0;

		if (ppu.scanline == 154)  // each frame is 154 scanlines
			ppu.scanline = 0;
	}

}

void PPU_write_LCDC(uint8_t value)
{
	ppu.LCDC = value;
}

void PPU_write_STAT(uint8_t value)
{
	ppu.STAT.reg = value;
}

void PPU_write_SCY(uint8_t value)
{
	ppu.SCY = value;
}

void PPU_write_SCX(uint8_t value)
{
	ppu.SCX = value;
}

void PPU_write_LYC(uint8_t value)
{
	ppu.LYC = value;
}

void PPU_write_BGP(uint8_t value)
{
	ppu.BGP = value;
}

void PPU_write_OBJP0(uint8_t value)
{
	ppu.OBJP0 = value;
}

void PPU_write_OBJP1(uint8_t value)
{
	ppu.OBJP1 = value;
}

void PPU_write_WY(uint8_t value)
{
	ppu.WY = value;
}

void PPU_write_WX(uint8_t value)
{
	ppu.WX = value;
}

uint8_t PPU_read_LCDC(void)
{
	return ppu.LCDC;
}

uint8_t PPU_read_STAT(void)
{
	return ppu.STAT.reg;
}

uint8_t PPU_read_SCY(void)
{
	return ppu.SCY;
}

uint8_t PPU_read_SCX(void)
{
	return ppu.SCX;
}

uint8_t PPU_read_LY(void)
{
	return ppu.LY;
}

uint8_t PPU_read_LYC(void)
{
	return ppu.LYC;
}

uint8_t PPU_read_WY(void)
{
	return ppu.WY;
}

uint8_t PPU_read_WX(void)
{
	return ppu.WX;
}


