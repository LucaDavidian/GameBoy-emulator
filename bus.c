#include <stdint.h>
#include "bus.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "joypad.h"
#include "serial.h"
#include "timer.h"
#include "cartridge.h"
#include "DMA.h"

static uint8_t WRAM[0x2000];
static uint8_t HRAM[0x80];
static uint8_t IE;
static uint8_t IF = 0xE0;

void bus_reset(void)
{

}

/**** bus interface ****/
uint8_t bus_read(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x7FFF)             ////////////// cartridge ROM - 32 KB
        if (address <= 0x00FF && cpu.boot)
            return cpu.bootROM[address & 0xFF];             ////////////// bootstrap ROM mapped to 0x00 - 0xFF - 256 Bytes
        else
            return cartridge_read(address);                 // bootstrap ROM disabled or address >= 0x100
    else if (address >= 0x8000 && address <= 0x9FFF)        ////////////// VRAM - 8KB
        return read_VRAM(address);
    else if (address >= 0xA000 && address <= 0xBFFF)        ////////////// external RAM - 8 KB
        ;
    else if (address >= 0xC000 && address <= 0xDFFF)        ////////////// work RAM - 8KB
        return WRAM[address & 0x1FFF];
    else if (address >= 0xE000 && address <= 0xFFFF)
        if (address >= 0xFE00 && address <= 0xFE9F)         ////////////// OAM - Object Attribute Memory table - 160 Bytes
            return read_OAM(address);  
        else if (address >= 0xFF00 && address <= 0xFF7F)    ////////////// IO memory mapped devices (128 bytes)
        {
            /**** joypad ****/
            if (address == 0xFF00)             // joypdad P1/JOY
                return joypad_read();

            else if (address == 0xFF01)        // serial SB
                return serial_read_SB();
            else if (address == 0xFF02)        // serial SC
                return serial_read_SC();

            /**** timer/counter ****/
            else if (address == 0xFF04)        // timer DIV
                return timer_read_DIV();
            else if (address == 0xFF05)        // timer TIMA
                return timer_read_TIMA();
            else if (address == 0xFF06)        // timer TMA 
                return timer_read_TMA();
            else if (address == 0xFF07)        // timer TAC
                return timer_read_TAC();

            /**** APU - sound controller ****/
            else if (address == 0xFF10)        // apu channel 1 sweep register NR10
                return APU_read_NR10();
            else if (address == 0xFF11)        // apu channel 1 sound length and wave pattern/duty cycle NR11
                return APU_read_NR11();
            else if (address == 0xFF12)        // apu channel 1 volume envelope NR12
                return APU_read_NR12();
            else if (address == 0xFF13)        // apu channel 1 frequency low NR13
                return APU_read_NR13();
            else if (address == 0xFF14)        // apu channel 1 frequency high NR14
                return APU_read_NR14();
            else if (address == 0xFF15)        // unused NR20  
                ;
            else if (address == 0xFF16)        // apu channel 2 sound length and wave pattern/duty cycle NR21
                APU_read_NR21();
            else if (address == 0xFF17)        // apu channel 2 volume envelope NR22
                APU_read_NR22();
            else if (address == 0xFF18)        // apu channel 2 frequency low NR23
                APU_read_NR23();
            else if (address == 0xFF19)        // apu channel 2 frequency high NR24
                APU_read_NR24();
            else if (address == 0xFF1A)        // apu channel 3 sound on/off NR30
                APU_read_NR30();
            else if (address == 0xFF1B)        // apu channel 3 sound length NR31
                APU_read_NR31();
            else if (address == 0xFF1C)        // apu channel 3 output level select NR32
                APU_read_NR32();
            else if (address == 0xFF1D)        // apu channel 3 frequency low NR33
                APU_read_NR33();
            else if (address == 0xFF1E)        // apu channel 3 frequency high NR34
                APU_read_NR34();
            else if (address == 0xFF1F)        // unused NR40
                ;
            else if (address == 0xFF20)        // apu channel 4 sound length NR41
                return APU_read_NR41();
            else if (address == 0xFF21)        // apu channel 4 volume envelope NR42
                return APU_read_NR42();
            else if (address == 0xFF22)        // apu channel 4 polynomial counter NR43
                return APU_read_NR43();
            else if (address == 0xFF23)        // apu channel 4 counter/consecutive and initial NR44
                return APU_read_NR44();
            else if (address == 0xFF24)        // apu channel control, on/off and volume NR50          
                return APU_read_NR50();
            else if (address == 0xFF25)        // sound output terminal selection NR51
                return APU_read_NR51();
            else if (address == 0xFF26)        // sound on/off NR52
                return APU_read_NR52();
            else if (address >= 0xFF27 && address <= 0xFF2F)  // apu unused
                ;
            else if (address >= 0xFF30 && address <= 0xFF3F)  // apu wave table RAM
                return APU_read_wave_table(address);

            /**** PPU - LCD controller ****/
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

            /**** interrupt flag register ****/
            else if (address == INT_FLAG_REG)  // IF Interrupt Flag register
                return IF;
        }
        else if (address >= 0xFF80 && address <= 0xFFFE)    ////////////// HRAM - 127 Bytes
            return HRAM[address & 0x007F];
        else if (address == INT_ENABLE_REG)                 ////////////// IE Interrupt Enable Register - 1 Byte
            return IE;
        else                                                // invalid memory access 
        {
            //printf("invalid memory access - read from 0x%x\n", address);
            return 0xFF;
        }
}

void bus_write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x7FFF)             ////////////// cartridge ROM - 32KB
        cartridge_write(address, data);
    else if (address >= 0x8000 && address <= 0x9FFF)        ////////////// VRAM - 8KB
        write_VRAM(address, data);
    else if (address >= 0xA000 && address <= 0xBFFF)        ////////////// external RAM - 8KB
        cartridge_write(address, data);
    else if (address >= 0xC000 && address <= 0xDFFF)        ////////////// work RAM - 8KB
        WRAM[address & 0x1FFF] = data;
    else if (address >= 0xE000 && address <= 0xFFFF)
        if (address >= 0xFE00 && address <= 0xFE9F)         ////////////// OAM - Object Attribute Memory table - 160 Bytes
            write_OAM(address, data);
        else if (address >= 0xFF00 && address <= 0xFF7F)    ////////////// 128 Bytes - IO memory mapped devices
        {
            /**** joypad ****/
            if (address == 0xFF00)             // joypad
                joypad_write(data);

            /**** serial ****/
            else if (address == 0xFF01)        // serial 
                serial_write_SB(data);
            else if (address == 0xFF02)        // serial SC
                serial_write_SC(data);

            /**** timer/counter ****/
            else if (address == 0xFF04)        // timer DIV
                timer_write_DIV(data);
            else if (address == 0xFF05)        // timer TIMA
                timer_write_TIMA(data);
            else if (address == 0xFF06)        // timer TMA 
                timer_write_TMA(data);
            else if (address == 0xFF07)        // timer TAC
                timer_write_TAC(data);

            /**** APU - sound controller ****/
            else if (address == 0xFF10)        // apu channel 1 sweep register NR10
                APU_write_NR10(data);
            else if (address == 0xFF11)        // apu channel 1 sound length and wave pattern/duty cycle NR11
                APU_write_NR11(data);
            else if (address == 0xFF12)        // apu channel 1 volume envelope NR12
                APU_write_NR12(data);
            else if (address == 0xFF13)        // apu channel 1 frequency low NR13
                APU_write_NR13(data);
            else if (address == 0xFF14)        // apu channel 1 frequency high NR14
                APU_write_NR14(data);
            else if (address == 0xFF15)        // apu unused NR20  
                ;
            else if (address == 0xFF16)        // apu channel 2 sound length and wave pattern/duty cycle NR21
                APU_write_NR21(data);
            else if (address == 0xFF17)        // apu channel 2 volume envelope NR22
                APU_write_NR22(data);
            else if (address == 0xFF18)        // apu channel 2 frequency low NR23
                APU_write_NR23(data);
            else if (address == 0xFF19)        // apu channel 2 frequency high NR24
                APU_write_NR24(data);
            else if (address == 0xFF1A)        // apu channel 3 sound on/off NR30
                APU_write_NR30(data);
            else if (address == 0xFF1B)        // apu channel 3 sound length NR31
                APU_write_NR31(data);
            else if (address == 0xFF1C)        // apu channel 3 output level select NR32
                APU_write_NR32(data);
            else if (address == 0xFF1D)        // apu channel 3 frequency low NR33
                APU_write_NR33(data);
            else if (address == 0xFF1E)        // apu channel 3 frequency high NR34
                APU_write_NR34(data);
            else if (address == 0xFF1F)        // apu unused NR40
                ;
            else if (address == 0xFF20)        // apu channel 4 sound length NR41
                APU_write_NR41(data);
            else if (address == 0xFF21)        // apu channel 4 volume envelope NR42
                APU_write_NR42(data);
            else if (address == 0xFF22)        // apu channel 4 polynomial counter NR43
                APU_write_NR43(data);
            else if (address == 0xFF23)        // apu channel 4 counter/consecutive and initial NR44
                APU_write_NR44(data);
            else if (address == 0xFF24)        // apu channel control, on/off and volume NR50          
                APU_write_NR50(data);
            else if (address == 0xFF25)        // sound output terminal selection NR51
                APU_write_NR51(data);
            else if (address == 0xFF26)        // sound on/off NR52
                APU_write_NR52(data);
            else if (address >= 0xFF27 && address <= 0xFF2F)  // apu unused
                ;
            else if (address >= 0xFF30 && address <= 0xFF3F)  // apu wave table RAM
                APU_write_wave_table(address, data);

            /**** PPU - LCD controller ****/
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
            else if (address == 0xFF46)        // DMA controller
                DMA_start(data);
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

            /**** bootstrap ROM disable ****/
            else if (address == 0xFF50)
            {
                if (data == 0x01)   // writing 1 to $FF50 disables bootstrap ROM
                    cpu.boot = 0;
            }

            /**** interrupt flag register ****/
            else if (address == INT_FLAG_REG)  // IF Interrupt Flag register
                IF = IF & 0xE0 | data & 0x1F;
        }
        else if (address >= 0xFF80 && address <= 0xFFFE)    ////////////// HRAM - 127 Bytes
            HRAM[address & 0x007F] = data;
        else if (address == INT_ENABLE_REG)                 ////////////// IE Interrupt Enable Register - 1 Byte
            IE = IE & 0xE0 | data & 0x1F;
        else                                                // invalid memory access 
            ;// printf("invalid memory access - write 0x%x to 0x%x\n", data, address);
}

/**** interrupts ****/
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