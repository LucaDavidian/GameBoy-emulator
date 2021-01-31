#include "CPU.h"
#include "PPU.h"
#include "cartridge.h"
#include "timer.h"
#include "SDL2/SDL.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVENTS) != 0)
    {
        printf("error initializing SDL: %s", SDL_GetError());
        return -1;
    }

    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));

    //cartridge_load(cartridge, "Tetris");
    
    /**** test ROMs ****/
    //cartridge_load(cartridge, "cpu_instrs");               // TODO: MBC1
    //cartridge_load(cartridge, "01-special");               // OK
    cartridge_load(cartridge, "02-interrupts");            // OK
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
    //cartridge_load(cartridge, "instr_timing");             // OK

    insert_cartridge(cartridge);

    // initialize emulator's systems
    CPU_init(&cpu);
    PPU_init();
    timer_init();

    // emulation loop
    int running = 1;
    while (running)  
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = 0;

        CPU_execute_machine_cycle(&cpu);

        PPU_clock();
        PPU_clock();
        PPU_clock();
        PPU_clock();
        
        timer_clock();
    }
    
    PPU_deinit();
    SDL_Quit();

    return 0;
}
