#include "cartridge.h"
#include <stdio.h>
#include <string.h>

int cartridge_load(Cartridge *cartridge, const char *rom_name)
{
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
    fread(&cartridge->mbc, 1, 1, rom);
    fread(&cartridge->ROM_size, 1, 1, rom);
    fread(&cartridge->RAM_size, 1, 1, rom);

    fseek(rom, 0, SEEK_SET);
    fread(cartridge->data, 32 * 1024 << cartridge->ROM_size, 1, rom);

    return 1;
}

uint8_t cartridge_read(Cartridge *cartridge, uint16_t address)
{
    return cartridge->data[address];
}