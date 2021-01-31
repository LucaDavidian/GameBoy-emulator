#include "PPU.h"
#include "bus.h"
#include "SDL2/SDL.h"
#include <stdint.h>

/**** PPU registers ****/
#define LCDC_POWER_BIT                   0x80

#define STAT_MODE_BITS                   0x03
#define STAT_COINCIDENCE_FLAG_BIT        0x04
#define STAT_MODE0_INTERRUPT_HBLANK_BIT  0x08
#define STAT_MODE1_INTERRUPT_VBLANK_BIT  0x10
#define STAT_MODE2_INTERRUPT_OAM_BIT     0x20
#define STAT_LYC_INTERRUPT_BIT           0x40

#define SCREEN_MODE0                        0
#define SCREEN_MODE1                        1
#define SCREEN_MODE2                        2
#define SCREEN_MODE3                        3

/**** display resolution ****/
#define DISPLAY_WIDTH                     160
#define DISPLAY_HEIGHT                    144

/**** VRAM ****/
#define VRAM_BASE                      0x8000
#define BG_TILE_DATA0                  0x8000  // 0x8000 - 0x87FF (256 x 16-bytes tiles = 4KB)
#define BG_TILE_DATA1                  0x8800  // 0x8800 - 0x97FF (256 x 16-bytes tiles = 4KB)
#define BG_TILE_MAP0                   0x9800  // 0x9800 - 0x9BFF (32 x 32 1-byte tile number = 1 KB)
#define BG_TILE_MAP1                   0x9C00  // 0x9C00 - 0x9FFF (32 x 32 1-byte tile number = 1 KB)

/**** PPU timings ****/
#define OAM_CLOCKS                               80
#define TRANFER_PLUS_HBLANK_CLOCKS              376
#define SCANLINE_CLOCKS                         456
#define VBLANK_SCANLINES                         10

uint32_t palette[] = { 0x009BBC0F, 0x008BAC0F, 0x00306230, 0x000F380F };

typedef enum PPU_State { PPU_STATE_OAM_SEARCH = 2, PPU_STATE_PIXEL_TRANSFER = 3, PPU_STATE_HBLANK = 0, PPU_STATE_VBLANK = 1 } PPU_State;

typedef struct PPU PPU;

struct PPU
{
	union
	{
		struct
		{
			uint8_t BG_enabled             : 1;    // enable background rendering (0: disabled, 1: enabled)
			uint8_t sprites_enabled        : 1;    // enable sprite rendering (0: disabled, 1: enabled)
			uint8_t sprite_size            : 1;    // select sprite size (0: 8x8 px, 1: 8x16 px)
			uint8_t BG_tile_map            : 1;    // select VRAM area to fetch background tile numbers from (0: 0x9800 - 0x9BFF, 1: 0x9C00 - 0x9FFF) - each area is 1 KB = 32 x 32 tiles 
			uint8_t BG_and_window_tileset  : 1;    // select VRAM area to fetch background and window tile data from (0: 0x8800 - 0x0x97FF, 1: 0x8000 - 0x8FFF) - each area is 4 KB = 256 16-bytes tiles
			uint8_t window_enable          : 1;    // enable window rendering
			uint8_t window_tile_map        : 1;    // select VRAM area to fetch window tile numbers from (0: 0x9800 - 0x9BFF, 1: 0x9C00 - 0x9FFF) - each area is 1 KB = 32 x 32 tiles 
			uint8_t LCD_power              : 1;    // turn on/off display (0: off, 1: on)
		} bits;

		uint8_t reg;
	} LCDC;           // LCD control (R/W)           - 0xFF40

	union
	{
		struct
		{
			uint8_t mode_flag              : 2;    // current mode (read-only) (0: HBLANK, 1: VBLANK, 2: OAM search, 3: pixel transfer) - CPU can access VRAM only in mode 0, 1 and 2; it can access OAM in mode 0 and 1 
			uint8_t coincidence_flag       : 1;    // comparison signal (read-only) (0: LY != LYC, 1: LY == LYC)
			uint8_t mode0_HBLANK_interrupt : 1;    // enable HBLANK interrupt (0: disabled, 1: enabled)
			uint8_t mode1_VBLANK_interrupt : 1;    // enable VBLANK interrupt (0: disabled, 1: enabled)
			uint8_t mode2_OAM_interrupt    : 1;    // enable OAM interrupt (0: disabled, 1: enabled)
			uint8_t LYC_interrupt          : 1;    // enable LYC interrupt (0: disabled, 1: enabled)
			uint8_t unused                 : 1;    
		} bits;

		uint8_t reg; 
	} STAT;           // LCD status (R/W)            - 0xFF41

	uint8_t SCY;      // scroll Y (R/W)              - 0xFF42
	uint8_t SCX;      // scroll X (R/W)              - 0xFF43
	uint8_t LY;       // current scanline (R)        - 0xFF44
	uint8_t LYC;      // LYC compare (R/W)           - 0xFF45
	uint8_t BGP;      // background palette data (W) - 0xFF47
	uint8_t OBJP0;    // sprite palette data 0 (W)   - 0xFF48
	uint8_t OBJP1;    // sprite palette data 1 (W)   - 0xFF49
	uint8_t WY;       // window y-coordinate (R/W)   - 0xFF4A
	uint8_t WX;       // window x-coordinate (R/W)   - 0xFF4B

	uint16_t cycle;   // @ 4.194304 MHz
	uint8_t pixels_pushed;

	uint8_t tile_number;
	uint16_t tile_data_address;
	uint8_t tile_data_low;
	uint8_t tile_data_high;

	int pixel_FIFO_stop;
	uint16_t pixel_FIFO_high;
	uint16_t pixel_FIFO_low;

	PPU_State state;
};

static PPU ppu;
static SDL_Window *window;
static SDL_Renderer *renderer;

static SDL_Texture *background_map;
static uint8_t background_buffer[256 * 256 * 4];

static SDL_Texture *window_map;
static uint8_t window_buffer[256 * 256 * 4];

static SDL_Texture *tile_data;
static uint8_t tile_buffer[16 * 24 * 64 * 4]; // 16 x 24 tiles, each 64 pixels, each pixel 4 bytes

static SDL_Texture *display;
static uint8_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT * 4];

void PPU_init(void)
{
	// initialize graphics system
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
	{
		printf("error initializing SDL: %s", SDL_GetError());
		return -1;
	}

	uint8_t paddingX = 60, paddingY = 20;
	window = SDL_CreateWindow("GameBoy Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, paddingX + DISPLAY_WIDTH * 4 + 8 * 8 + 256 * 2, paddingY + DISPLAY_HEIGHT * 4, 0);
	if (!window)
	{
		printf("error creating window: %s", SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		printf("error creating renderer: %s", SDL_GetError());
		return -1;
	}

	display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if (!display)
	{
		printf("error creating texture: %s", SDL_GetError());
		return -1;
	}

	background_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	if (!display)
	{
		printf("error creating texture: %s", SDL_GetError());
		return -1;
	}

	window_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	if (!display)
	{
		printf("error creating texture: %s", SDL_GetError());
		return -1;
	}

	tile_data = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 16 * 8, 24 * 8);
	if (!display)
	{
		printf("error creating texture: %s", SDL_GetError());
		return -1;
	}

	// initialize PPU
	ppu.cycle = 0;
	ppu.pixels_pushed = 0;
	ppu.LY = 0x00;

	ppu.pixel_FIFO_stop = 0;
	ppu.pixel_FIFO_high = 0;
	ppu.pixel_FIFO_low = 0;
}

void PPU_deinit(void)
{
	SDL_DestroyTexture(display);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void render_VRAM(void)
{
	// render tiles
	for (int i = 0; i < 24; i++)
		for (int h = 0; h < 16; h++)
		{
			uint16_t tile_base_adddress = VRAM_BASE + (h + i * 16) * 16;

			for (int j = 0; j < 16; j += 2)
			{
				uint8_t tile_data_low = bus_read(tile_base_adddress + j);
				uint8_t tile_data_high = bus_read(tile_base_adddress + j + 1);

				for (int k = 0; k < 8; k++)
				{
					uint8_t pixel_palette_low = tile_data_low >> 7 - k & 0x01;
					uint8_t pixel_palette_high = tile_data_high >> 7 - k & 0x01;

					uint8_t pixel_color_index = pixel_palette_low | pixel_palette_high << 1;
					uint8_t pixel_palette = ppu.BGP >> pixel_color_index * 2 & 0x03;
					uint32_t pixel_color = palette[pixel_palette];

					*(uint32_t*)(tile_buffer + (h * 8 + k + (i * 8 + j/2) * 16 * 8) * 4) = pixel_color;
			}
		}
	}

	// render background VRAM
	uint16_t background_tile_map_base = ppu.LCDC.bits.BG_tile_map ? BG_TILE_MAP1 : BG_TILE_MAP0;
	uint16_t background_tile_data_base = ppu.LCDC.bits.BG_and_window_tileset ? BG_TILE_DATA0 : BG_TILE_DATA1;

	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 32; j++)
		{
			uint8_t tile_number = bus_read(background_tile_map_base + j + i * 32);
			uint16_t tile_data_address = background_tile_data_base + tile_number * 16;
			if (background_tile_data_base == BG_TILE_DATA1)
			{ 
				tile_data_address += 0x800;   // TODO: wrong! * 16 
				if (tile_data_address >= 0x97FF)
					tile_data_address -= 0x1000;
			}

			for (int k = 0; k < 16; k += 2)
			{
				uint8_t tile_data_low = bus_read(tile_data_address + k);
				uint8_t tile_data_high = bus_read(tile_data_address + k + 1);

				for (int l = 0; l < 8; l++)
				{
					uint8_t low_bit = tile_data_low >> 7 - l & 0x01;
					uint8_t high_bit = tile_data_high >> 7 - l & 0x01;
					uint8_t pixel_palette = low_bit | high_bit << 1;

					uint8_t pixel_color = ppu.BGP >> pixel_palette * 2 & 0x03;
					*(uint32_t*)(background_buffer + (j * 8 + l + (i * 8 + k/2) * 32 * 8) * 4) = palette[pixel_color];
				}
			}
		}

	// render window VRAM
	uint16_t window_tile_map_base = ppu.LCDC.bits.window_tile_map ? BG_TILE_MAP1 : BG_TILE_MAP0;
	uint16_t window_tile_data_base = ppu.LCDC.bits.BG_and_window_tileset ? BG_TILE_DATA0 : BG_TILE_DATA1;

	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 32; j++)
		{
			uint8_t tile_number = bus_read(window_tile_map_base + j + i * 32);
			uint16_t tile_data_address = window_tile_data_base + tile_number * 16;
			if (window_tile_data_base == BG_TILE_DATA1)
			{  
				tile_data_address += 0x800;   // TODO: wrong! * 16 
				if (tile_data_address >= 0x97FF)     
					tile_data_address -= 0x1000;
			}

			for (int k = 0; k < 16; k += 2)
			{
				uint8_t tile_data_low = bus_read(tile_data_address + k);
				uint8_t tile_data_high = bus_read(tile_data_address + k + 1);

				for (int l = 0; l < 8; l++)
				{
					uint8_t low_bit = tile_data_low >> 7 - l & 0x01;
					uint8_t high_bit = tile_data_high >> 7 - l & 0x01;
					uint8_t pixel_palette = low_bit | high_bit << 1;

					uint8_t pixel_color = ppu.BGP >> pixel_palette * 2 & 0x03;
					*(uint32_t*)(window_buffer + (j * 8 + l + (i * 8 + k / 2) * 32 * 8) * 4) = palette[pixel_color];
				}
			}
		}
}

void PPU_clock(void)  
{
	switch (ppu.state)
	{
		case PPU_STATE_OAM_SEARCH:  // mode 2: OAM memory search
			if (ppu.cycle == 0)
			{
				ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~STAT_MODE_BITS | SCREEN_MODE2;

				if (check_int_enabled(INT_LCD_STAT) && ppu.STAT.bits.mode2_OAM_interrupt)
					set_int_flag(INT_LCD_STAT);
			}

			ppu.cycle++;

			if (ppu.cycle == OAM_CLOCKS)
				ppu.state = PPU_STATE_PIXEL_TRANSFER;

			break;
		case PPU_STATE_PIXEL_TRANSFER:  // mode 3: pixel transfer
			if (ppu.cycle == OAM_CLOCKS)
				ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~STAT_MODE_BITS | SCREEN_MODE3;

			uint8_t pixelX = ppu.pixels_pushed;
			uint8_t pixelY = ppu.LY;
			
			switch (ppu.cycle % 8)  // fetcher
			{
				case 0:
					break; // idle
				case 1:    // push 8 pixels to FIFO
					ppu.pixel_FIFO_low = ppu.pixel_FIFO_low & 0x00FF | (uint16_t)ppu.tile_data_low << 8;
					ppu.pixel_FIFO_high = ppu.pixel_FIFO_high & 0x00FF | (uint16_t)ppu.tile_data_high << 8;
					ppu.pixel_FIFO_stop = 0;
					break;
				case 2:    // read tile number (32 x 32 tile map)
				{
					uint16_t tile_map_base = ppu.LCDC.bits.BG_tile_map ? BG_TILE_MAP1 : BG_TILE_MAP0;
					uint8_t tileX = (pixelX + ppu.SCX) / 8 % 32;  // unsigned overflow - wrap around
					uint8_t tileY = (pixelY /*+ ppu.SCY*/) / 8 % 32;  // unsigned overflow - wrap around
					uint16_t tile_map_address = tile_map_base + tileX + tileY * 32;
					ppu.tile_number = bus_read(tile_map_address);
				}
					break;
				case 4:    // read tile data low 
				{
					uint16_t tile_data_base = ppu.LCDC.bits.BG_and_window_tileset ? BG_TILE_DATA0 : BG_TILE_DATA1;
					ppu.tile_data_address = tile_data_base + ppu.tile_number * 16;
					if (tile_data_base == BG_TILE_DATA1)
					{
						ppu.tile_data_address += 0x0800;  // TODO: wrong! * 16 
						if (ppu.tile_data_address > 0x97FF)
							ppu.tile_data_address -= 0x1000;
					}
					ppu.tile_data_low = bus_read(ppu.tile_data_address);
				}
					break;
				case 6:    // read tile data high
					ppu.tile_data_address++;
					ppu.tile_data_high = bus_read(ppu.tile_data_address);
					break;
			}

			ppu.cycle++;

			if (!ppu.pixel_FIFO_stop)
			{
				uint8_t pixel_palette = ppu.pixel_FIFO_low & 0x01 | (ppu.pixel_FIFO_high & 0x01) << 1;
				uint8_t pixel_color = ppu.BGP >> pixel_palette * 2 & 0x03;
				*(uint32_t*)(buffer + ppu.pixels_pushed * 4 + ppu.LY * DISPLAY_WIDTH * 4) = palette[pixel_color];

				ppu.pixel_FIFO_low >>= 1;
				ppu.pixel_FIFO_high >>= 1;
				ppu.pixels_pushed++;
			}

			if (ppu.pixels_pushed == DISPLAY_WIDTH)
			{
				ppu.pixels_pushed = 0;
				ppu.state = PPU_STATE_HBLANK;
			}

			break;
		case PPU_STATE_HBLANK:  // mode 0: in HBLANK
			if ((ppu.STAT.bits.mode_flag & STAT_MODE_BITS) != SCREEN_MODE0)
			{
				ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~STAT_MODE_BITS | SCREEN_MODE0;

				if (check_int_enabled(INT_LCD_STAT) && ppu.STAT.bits.mode0_HBLANK_interrupt)
					set_int_flag(INT_LCD_STAT);
			}

			ppu.cycle++;

			if (ppu.cycle == SCANLINE_CLOCKS)
			{
				render_VRAM();
				PPU_render();
			}

			if (ppu.cycle == SCANLINE_CLOCKS) 
			{
				ppu.LY++;
				ppu.cycle = 0;

				if (ppu.LY == ppu.LYC)
				{
					ppu.STAT.bits.coincidence_flag = 1;

					if (ppu.STAT.bits.LYC_interrupt)
						set_int_flag(INT_LCD_STAT);
				}
				else
					ppu.STAT.bits.coincidence_flag = 0;

				if (ppu.LY == DISPLAY_HEIGHT)
					ppu.state = PPU_STATE_VBLANK;
				else
					ppu.state = PPU_STATE_OAM_SEARCH;
			}

			break;
		case PPU_STATE_VBLANK:  // mode 1: in VBLANK
			if (ppu.cycle == 0)
			{
				ppu.STAT.bits.mode_flag = ppu.STAT.bits.mode_flag & ~STAT_MODE_BITS | SCREEN_MODE1;

				if (check_int_enabled(INT_VBLANK) && ppu.STAT.bits.mode1_VBLANK_interrupt)
					set_int_flag(INT_VBLANK);  			
			}

			ppu.cycle++;

			if (ppu.cycle == SCANLINE_CLOCKS)
			{
				ppu.LY++;
				ppu.cycle = 0;

				if (ppu.LY == ppu.LYC)
				{
					ppu.STAT.bits.coincidence_flag = 1;

					if (ppu.STAT.bits.LYC_interrupt)
						set_int_flag(INT_LCD_STAT);
				}
				else
					ppu.STAT.bits.coincidence_flag = 0;

				if (ppu.LY > 153)
				{
					ppu.LY = 0;
					ppu.state = PPU_STATE_OAM_SEARCH;
				}
			}

			break;
	}
}

void PPU_render(void)
{
	SDL_SetRenderDrawColor(renderer, 0xD0, 0xD0, 0xD0, 0x00);
	SDL_RenderClear(renderer);

	SDL_UpdateTexture(display, NULL, (const void*)buffer, DISPLAY_WIDTH * 4);
	SDL_Rect display_rect = { 10, 10, DISPLAY_WIDTH * 4, DISPLAY_HEIGHT * 4 };
	SDL_RenderCopy(renderer, display, NULL, &display_rect);

	SDL_UpdateTexture(background_map, NULL, (const void*)background_buffer, 256 * 4);
	SDL_Rect background_map_rect = { 10 + DISPLAY_WIDTH * 4 + 10, 10, 256 * 1.1, 256 * 1.1};
	SDL_RenderCopy(renderer, background_map, NULL, &background_map_rect);

	SDL_UpdateTexture(window_map, NULL, (const void*)window_buffer, 256 * 4);
	SDL_Rect window_map_rect = { 10 + DISPLAY_WIDTH * 4 + 10, 10 + 256 * 1.1 + 10, 256 * 1.1 , 256 * 1.1};
	SDL_RenderCopy(renderer, window_map, NULL, &window_map_rect);

	SDL_UpdateTexture(tile_data, NULL, (const void*)tile_buffer, 16 * 8 * 4);
	SDL_Rect tile_data_rect = { 20 + DISPLAY_WIDTH * 4 + 10 + 256 * 1.1, 10 , 16 * 8 *2, 24 * 8 *2};
	SDL_RenderCopy(renderer,tile_data, NULL, &tile_data_rect);

	SDL_RenderPresent(renderer);
}

void PPU_write_LCDC(uint8_t value)
{
	if (!(value & LCDC_POWER_BIT))
		ppu.LY = 0x00;

	ppu.LCDC.reg = value;
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
	return ppu.LCDC.reg;
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

