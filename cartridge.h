#include <stdint.h>

typedef struct Cartridge
{
    uint8_t data[0x8000];  // 32 KB cartridge ROM address space 0x0000 - 0x7FFF
    char title[0x10];
    uint8_t mbc;
    uint8_t ROM_size;
    uint8_t RAM_size;
} Cartridge;

int cartridge_load(Cartridge *cartridge, const char *rom_name);
uint8_t cartridge_read(Cartridge *cartridge, uint16_t address);