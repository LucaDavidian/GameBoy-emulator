#include "CPU.h"
#include "cartridge.h"
#include <stdlib.h>
#include "bus.h"

int main(int argc, char *argv[])
{
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    cartridge_load(cartridge, "Tetris");
    //cartridge_load(cartridge, "09-op r,r");
    insert_cartridge(cartridge);

    CPU_init(&cpu);

    // emulation loop
    while (1)
    {
        CPU_execute_machine_cycle(&cpu);
    }
    
    return 0;
}
