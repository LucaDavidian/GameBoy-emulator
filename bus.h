#include <stdint.h>

uint8_t bus_read(uint16_t address);
void bus_write(uint16_t address, uint8_t data);

struct Cartridge;
void insert_cartridge(struct Cartridge *);