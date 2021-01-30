#include "serial.h"

typedef struct Serial
{
    uint8_t SB;    // serial transfer data (R/W)    - 0xFF01
    uint8_t SC;    // serial transfer control (R/W) - 0xFF02
} Serial;

Serial serial;

void serial_write_SB(uint8_t data)
{
    serial.SB = data;
}

void serial_write_SC(uint8_t data)
{
    serial.SC = data;

    if (data & 0x81)
        printf("%c", serial.SB);  // debug test ROM - print to console
}

uint8_t serial_read_SB(void)
{
    return serial.SB;
}

uint8_t serial_read_SC(void)
{
    return serial.SC;
}