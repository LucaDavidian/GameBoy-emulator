#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

typedef struct Serial
{
    uint8_t SB;  // serial transfer data

    union
    {
        struct
        {
            uint8_t clock_select : 1;
            uint8_t : 6;
            uint8_t transfer_start : 1;
        } bits;

        uint8_t reg;
    } SC;  // serial transfer control
} Serial;

extern Serial serial;

void serial_write(uint8_t data);

#endif