#include <stdint.h>

int cartridge_load(const char *rom_name);

uint8_t cartridge_read(uint16_t address);
void cartridge_write(uint16_t address, uint8_t data);