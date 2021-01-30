#include "CPU.h"
#include "cartridge.h"
#include <stdlib.h>
#include "timer.h"
#include "SDL2/SDL.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        printf("error initializing SDL: %s", SDL_GetError());
        return -1;
    }
    

    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));

    //cartridge_load(cartridge, "Tetris");
    
    //cartridge_load(cartridge, "cpu_instrs");               // TODO: MBC1
    //cartridge_load(cartridge, "01-special");               // OK
    //cartridge_load(cartridge, "02-interrupts");            // OK
    //cartridge_load(cartridge, "03-op sp,hl");              // OK
    //cartridge_load(cartridge, "04-op r,imm");              // OK
    //cartridge_load(cartridge, "05-op rp");                 // OK
    //cartridge_load(cartridge, "06-ld r,r");                // OK
    //cartridge_load(cartridge, "07-jr,jp,call,ret,rst");    // OK
    //cartridge_load(cartridge, "08-misc instrs");           // OK
    //cartridge_load(cartridge, "09-op r,r");                // OK
    //cartridge_load(cartridge, "10-bit ops");               // OK
    //cartridge_load(cartridge, "11-op a,(hl)");             // OK
    
    //cartridge_load(cartridge, "interrupt_time");           // TODO: MBC2
    cartridge_load(cartridge, "instr_timing");             // OK

    insert_cartridge(cartridge);

    CPU_init(&cpu);
    timer_init();

    // emulation loop
    while (1)
    {
        CPU_execute_machine_cycle(&cpu);
        timer_clock();
    }
    

    SDL_Quit();

    return 0;
}
