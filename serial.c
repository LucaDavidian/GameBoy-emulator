#include "serial.h"

Serial serial;

void serial_write(uint8_t data)
{
    printf("%x", data);
}