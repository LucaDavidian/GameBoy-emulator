#include <stdint.h>
#include "bus.h"
#include "CPU.h"
#include "cartridge.h"
#include "serial.h"
#include "timer.h"
#include "PPU.h"

uint8_t WRAM[0x2000];
uint8_t VRAM[0x2000];
uint8_t OAM[0x80 + 0x20];
uint8_t HRAM[0x80];
uint8_t IE;
uint8_t IF;
Cartridge *cartridge;

void bus_reset(void)
{
    /*[$FF05] = $00; TIMA
        [$FF06] = $00; TMA
        [$FF07] = $00; TAC
        [$FF10] = $80; NR10
        [$FF11] = $BF; NR11
        [$FF12] = $F3; NR12
        [$FF14] = $BF; NR14
        [$FF16] = $3F; NR21
        [$FF17] = $00; NR22
        [$FF19] = $BF; NR24
        [$FF1A] = $7F; NR30
        [$FF1B] = $FF; NR31
        [$FF1C] = $9F; NR32
        [$FF1E] = $BF; NR34
        [$FF20] = $FF; NR41
        [$FF21] = $00; NR42
        [$FF22] = $00; NR43
        [$FF23] = $BF; NR44
        [$FF24] = $77; NR50
        [$FF25] = $F3; NR51
        [$FF26] = $F1 - GB, $F0 - SGB; NR52
        [$FF40] = $91; LCDC
        [$FF42] = $00; SCY
        [$FF43] = $00; SCX
        [$FF45] = $00; LYC
        [$FF47] = $FC; BGP
        [$FF48] = $FF; OBP0
        [$FF49] = $FF; OBP1
        [$FF4A] = $00; WY
        [$FF4B] = $00; WX
        [$FFFF] = $00; IE*/
}

#include <stdio.h>

uint8_t bus_read(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x7FFF)             // 32 KB cartridge ROM 
        if (address <= 0x00FF && cpu.boot)
            return cpu.bootROM[address & 0xFF];                   // 256 bytes bootstrap ROM mapped to 0x00 - 0xFF 
        else
            return cartridge_read(cartridge, address);            // bootstrap ROM disabled or address >= 0x100
    else if (address >= 0x8000 && address <= 0x9FFF)        // 8 KB VRAM
        return VRAM[address & 0x1FFF];
    else if (address >= 0xA000 && address <= 0xBFFF)        // 8 KB external RAM
        ;
    else if (address >= 0xC000 && address <= 0xDFFF)        // 8 KB work RAM
        return WRAM[address & 0x1FFF];
    else if (address >= 0xE000 && address <= 0xFFFF)
        if (address >= 0xFE00 && address <= 0xFE9F)         // OAM - object attribute table
            return OAM[address & 0x00FF];  
        else if (address >= 0xFF00 && address <= 0xFF7F)    // IO memory mapped devices (128 bytes)
        {
            if (address == 0xFF01)             // serial SB
                return serial_read_SB();
            else if (address == 0xFF02)        // serial SC
                return serial_read_SC();
            else if (address == 0xFF04)        // timer DIV
                return timer_read_DIV();
            else if (address == 0xFF05)        // timer TIMA
                return timer_read_TIMA();
            else if (address == 0xFF06)        // timer TMA 
                return timer_read_TMA();
            else if (address == 0xFF07)        // timer TAC
                return timer_read_TAC();
            else if (address == 0xFF40)        // ppu LCDC
                return PPU_read_LCDC();
            else if (address == 0xFF41)        // ppu STAT
                return PPU_read_STAT();
            else if (address == 0xFF42)        // ppu SCY
                return PPU_read_SCY();
            else if (address == 0xFF43)        // ppu SCX
                return PPU_read_SCX();
            else if (address == 0xFF44)        // ppu LY
                return PPU_read_LY();
            else if (address == 0xFF45)        // ppu LYC
                return PPU_read_LYC();
            else if (address == 0xFF4A)        // ppu WY
                return PPU_read_WY();
            else if (address == 0xFF4B)        // ppu WX
                return PPU_read_WX();
            else if (address == INT_FLAG_REG)  // IF Interrupt Flag register
                return IF;
        }
        else if (address >= 0xFF80 && address <= 0xFFFE)    // HRAM
            return HRAM[address & 0x007F];
        else if (address == INT_ENABLE_REG)                 // IE Interrupt Enable Register
            return IE;
        //else                                                // invalid memory access 
        //    return printf("invalid memory access: 0x%x\n", address), 0;
}

void bus_write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x7FFF)             // 32 KB cartridge ROM 
        ;
    else if (address >= 0x8000 && address <= 0x9FFF)        // 8 KB VRAM
        VRAM[address & 0x1FFF] = data;
    else if (address >= 0xA000 && address <= 0xBFFF)        // 8 KB external RAM
        ;
    else if (address >= 0xC000 && address <= 0xDFFF)        // 8 KB work RAM
        WRAM[address & 0x1FFF] = data;
    else if (address >= 0xE000 && address <= 0xFFFF)
        if (address >= 0xFE00 && address <= 0xFE9F)         // OAM - object attribute table
            ;
        else if (address >= 0xFF00 && address <= 0xFF7F)    // IO memory mapped devices (128 bytes)
        {
            if (address == 0xFF01)             // serial 
                serial_write_SB(data);
            else if (address == 0xFF02)        // serial SC
                serial_write_SC(data);
            else if (address == 0xFF04)        // timer DIV
                timer_write_DIV(data);
            else if (address == 0xFF05)        // timer TIMA
                timer_write_TIMA(data);
            else if (address == 0xFF06)        // timer TMA 
                timer_write_TMA(data);
            else if (address == 0xFF07)        // timer TAC
                timer_write_TAC(data);
            else if (address == 0xFF40)        // ppu LCDC
                PPU_write_LCDC(data);
            else if (address == 0xFF41)        // ppu STAT
                PPU_write_STAT(data);
            else if (address == 0xFF42)        // ppu SCY
                PPU_write_SCY(data);
            else if (address == 0xFF43)        // ppu SCX
                PPU_write_SCX(data);
            else if (address == 0xFF45)        // ppu LYC
                PPU_write_LYC(data);
            else if (address == 0xFF47)        // ppu BGP        
                PPU_write_BGP(data);
            else if (address == 0xFF48)        // ppu OBJP0
                PPU_write_OBJP0(data);
            else if (address == 0xFF49)        // ppu OBJP1
                PPU_write_OBJP1(data);
            else if (address == 0xFF4A)        // ppu WY
                PPU_write_WY(data);
            else if (address == 0xFF4B)        // ppu WX
                PPU_write_WX(data);
            else if (address == INT_FLAG_REG)  // IF Interrupt Flag register
                IF = data; 
        }
		else if (address >= 0xFF80 && address <= 0xFFFE)    // HRAM
			HRAM[address & 0x007F] = data;
		else if (address == INT_ENABLE_REG)                 // IE Interrupt Enable Register
			IE = data;
		//else                                                // invalid memory access 
  //          printf("invalid memory access: 0x%x\n", address);
}

void set_int_flag(enum Int_Flag interrupt)
{
    IF |= interrupt;
}

void clear_int_flag(enum Int_Flag interrupt)
{
    IF &= ~interrupt;
}

int check_int_flag(enum Int_Flag interrupt)
{
    return IF & interrupt;
}

void enable_int(enum Int_Flag interrupt)
{
    IE |= interrupt;
}

void disable_int(enum Int_Flag interrupt)
{
    IE &= ~interrupt;
}

int check_int_enabled(enum Int_Flag interrupt)
{
    return IE & interrupt;
}

void insert_cartridge(struct Cartridge *c)
{
    cartridge = c;
}