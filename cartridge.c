#include "cartridge.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum MBC
{
    ROM_ONLY = 0, MBC1, MBC1_RAM, MBC1_RAM_BATTERY, MBC2,
};

typedef struct Cartridge
{
    uint8_t *data;  // 32 KB cartridge ROM address space 0x0000 - 0x7FFF
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
    cartridge->data = (uint8_t*)malloc(32 * 1024 << cartridge->ROM_size);
    fread(cartridge->data, 32 * 1024 << cartridge->ROM_size, 1, rom);

    return 1;
}

uint8_t cartridge_read(uint16_t address)
{
    return cartridge->data[address];
}

void cartridge_write(uint16_t address, uint8_t data)
{
    switch (cartridge->MBC)
    {
        case ROM_ONLY:
            break;
        case MBC1:

            break;
    }
}