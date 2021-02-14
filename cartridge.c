#include "cartridge.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum MBC
{
    ROM_ONLY = 0, MBC1, MBC1_RAM, MBC1_RAM_BATTERY, MBC2 = 5, MBC2_BATTERY,
};

typedef struct Cartridge
{
    uint8_t *ROM;  // 32 KB cartridge ROM address space 0x0000 - 0x7FFF
    uint8_t *RAM;  
    char title[0x10];
    enum MBC MBC;
    uint8_t ROM_size;
    uint8_t RAM_size;
} Cartridge;

static Cartridge *cartridge = NULL;

int cartridge_load(const char *rom_name)
{
    cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    
    char rom_path[100] = "ROMs/";
    strcat(rom_path, rom_name);
    strcat(rom_path, ".gb");

    FILE *rom = fopen(rom_path, "rb");
    if (!rom)
    {
        printf("error loading ROM");
        return 0;
    }

    fseek(rom, 0x134, SEEK_SET);
    fread(cartridge->title, 16, 1, rom);
    fseek(rom, 3, SEEK_CUR);
    uint8_t MBC;
    fread(&MBC, 1, 1, rom);
    cartridge->MBC = MBC;
    fread(&cartridge->ROM_size, 1, 1, rom);
    fread(&cartridge->RAM_size, 1, 1, rom);

    fseek(rom, 0, SEEK_SET);
    cartridge->ROM = (uint8_t*)malloc(32 * 1024 << cartridge->ROM_size);
    fread(cartridge->ROM, 32 * 1024 << cartridge->ROM_size, 1, rom);

    switch (cartridge->RAM_size)
    {
        case 0:   // no RAM
            cartridge->RAM = NULL;  
            break;
        case 1:   // 2 KB RAM (partial bank)
            cartridge->RAM = (uint8_t*)malloc(cartridge->RAM, 2 * 1024);
            break; 
        case 2:   // 8 KB RAM (1 bank)
            cartridge->RAM = (uint8_t*)malloc(cartridge->RAM, 8 * 1024);
            break;
        case 3:   // 32 KB RAM (4 banks)
            cartridge->RAM = (uint8_t*)malloc(cartridge->RAM, 32 * 1024);
            break;
        case 4:   // 128 KB RAM (16 banks)
            cartridge->RAM = (uint8_t*)malloc(cartridge->RAM, 128 * 1024);
            break;
        case 5:   // 64 KB RAM (8 banks)
            cartridge->RAM = (uint8_t*)malloc(cartridge->RAM, 64 * 1024);
            break;
    }
    
    return 1;
}

// MBC1
static uint8_t ROM_bank = 1;  
static enum Banking_Mode { ROM_BANKING_MODE, RAM_BANKING_MODE } banking_mode;
static int RAM_enabled = 0;
static uint8_t RAM_bank = 0;

uint8_t cartridge_read(uint16_t address)
{
    switch (cartridge->MBC)
    {
        case ROM_ONLY:
            return cartridge->ROM[address];

            break;

        case MBC1:
            if (address >= 0x0000 && address <= 0x3FFF)
                return cartridge->ROM[address];
            else
            {
                uint32_t ROM_bank_address = ROM_bank * 0x4000;
                return cartridge->ROM[ROM_bank_address + (address & 0x3FFF)];
            }

            break;

        case MBC2_BATTERY:
            if (address >= 0x0000 && address <= 0x3FFF)
                return cartridge->ROM[address];
            else
            {
                uint32_t ROM_bank_address = ROM_bank * 0x4000;
                return cartridge->ROM[ROM_bank_address + (address & 0x3FFF)];
            }

            break;
    }
}

void cartridge_write(uint16_t address, uint8_t data)
{
    switch (cartridge->MBC)
    {
        case ROM_ONLY:

            break;

        case MBC1:  // max 2 MB ROM + 32 KB RAM
            if (address >= 0x0000 && address <= 0x1FFF)
            {
                if ((data & 0x0F) == 0x0A)
                    RAM_enabled = 1;
                else
                    RAM_enabled = 0;
            }
            else if (address >= 0x2000 && address <= 0x3FFF)
            {
                ROM_bank = ROM_bank & 0x60 | data & 0x1F;     // low 5 bits of ROM bank number

                if (data == 0x00)
                    ROM_bank = ROM_bank & 0x60 | 0x01;
            }
            else if (address >= 0x4000 && address <= 0x5FFF)
            {
                if (banking_mode == ROM_BANKING_MODE)
                    ROM_bank = ROM_bank & 0x1F | (data & 0x03) << 5;     // high 2 bits of ROM bank number
                else  // RAM_BANKING_MODE
                    RAM_bank = data & 0x03;                    
            }
            else if (address >= 0x6000 && address <= 0x7FFF)
            {
                if ((data & 0x01) == 0)
                    banking_mode = ROM_BANKING_MODE;
                else
                    banking_mode = RAM_BANKING_MODE;
            }

            break;

        case MBC2_BATTERY:
            if (address >= 0x0000 && address <= 0x3FFF)
            {
                if ((address & 0x0100) == 0)
                {
                    if (data == 0x0A)
                        ; // enable RAM
                }
                else // select ROM bank
                    if (data == 0)
                        ROM_bank = 1;
                    else
                        ROM_bank = data & 0x0F;
            }

            break;
    }
}