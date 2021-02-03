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

    cartridge_load("Tetris");
    //cartridge_load("Dr. Mario");
    
    /**** test ROMs ****/
    //cartridge_load("cpu_instrs");                 // TODO: MBC1
    //cartridge_load("01-special");                 // OK
    //cartridge_load("02-interrupts");              // OK
    //cartridge_load("03-op sp,hl");                // OK
    //cartridge_load("04-op r,imm");                // OK
    //cartridge_load("05-op rp");                   // OK
    //cartridge_load("06-ld r,r");                  // OK
    //cartridge_load("07-jr,jp,call,ret,rst");      // OK
    //cartridge_load("08-misc instrs");             // OK
    //cartridge_load("09-op r,r");                  // OK
    //cartridge_load("10-bit ops");                 // OK
    //cartridge_load("11-op a,(hl)");               // OK

    //cartridge_load("instr_timing");               // OK

    //cartridge_load("interrupt_time");             
    
    //cartridge_load("01-read_timing");
    //cartridge_load("02-write_timing");            // OK
    //cartridge_load("03-modify_timing");

    /**** initialize emulator's systems ****/
    CPU_init(&cpu);
    PPU_init();
    timer_init();

    /**** emulation loop ****/
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
