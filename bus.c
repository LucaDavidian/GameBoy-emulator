#include <stdint.h>
#include "bus.h"
#include "CPU.h"
#include "cartridge.h"
#include "serial.h"

uint8_t WRAM[0x2000];
uint8_t VRAM[0x2000];
uint8_t OAM[0x80 + 0x20];
uint8_t IE;
Cartridge *cartridge;

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
            if (address == 0xFF01)
                return serial.SB;
            else if (address == 0xFF02)
                return serial.SC.reg;
        }
        else if (address >= 0xFF80 && address <= 0xFFFE)    // HRAM
            ;
        else if (address == 0xFFFF)                         // IE Interrupt Enable Register
            return IE;
        else                                                // invalid memory access 
            ; 
}

void bus_write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x7FFF)             // 32 KB cartridge ROM 
        ;
    else if (address >= 0x8000 && address <= 0x9FFF)        // 8 KB VRAM
        ;
    else if (address >= 0xA000 && address <= 0xBFFF)        // 8 KB external RAM
        ;
    else if (address >= 0xC000 && address <= 0xDFFF)        // 8 KB work RAM
        ;
    else if (address >= 0xE000 && address <= 0xFFFF)
        if (address >= 0xFE00 && address <= 0xFE9F)         // OAM - object attribute table
            ;  
        else if (address >= 0xFF00 && address <= 0xFF7F)    // IO memory mapped devices (128 bytes)
            if (address == 0xFF01)
                serial.SB = data;
            else if (address == 0xFF02)
            {
                serial.SC.reg = data;
                if (data & 0x80)
                    serial_write(data);  // debug test ROM - print to console
            }
        else if (address >= 0xFF80 && address <= 0xFFFE)    // HRAM
            ;
        else if (address == 0xFFFF)                         // IE Interrupt Enable Register
            ;
        else                                                // invalid memory access 
            ; 
}

void insert_cartridge(struct Cartridge *c)
{
    cartridge = c;
}