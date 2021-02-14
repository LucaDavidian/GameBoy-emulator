#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "DMA.h"
#include "timer.h"
#include "SDL2/SDL.h"
#include <stdlib.h>

void clock(void);

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVENTS) != 0)
    {
        printf("error initializing SDL: %s", SDL_GetError());
        return -1;
    }
  
    /**** CPU test ROMs ****/
    //cartridge_load("Test ROMs/cpu_instrs/individual/cpu_instrs");                
    //cartridge_load("Test ROMs/cpu_instrs/individual/01-special");                 // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/02-interrupts");              // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/03-op sp,hl");                // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/04-op r,imm");                // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/05-op rp");                   // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/06-ld r,r");                  // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/07-jr,jp,call,ret,rst");      // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/08-misc instrs");             // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/09-op r,r");                  // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/10-bit ops");                 // OK
    //cartridge_load("Test ROMs/cpu_instrs/individual/11-op a,(hl)");               // OK
    //cartridge_load("Test ROMs/instr_timing/instr_timing");                        // OK
    //cartridge_load("Test ROMs/interrupt_time/interrupt_time");              
    //cartridge_load("Test ROMs/mem_timing/individual/01-read_timing");
    //cartridge_load("Test ROMs/mem_timing/individual/02-write_timing");            // OK
    //cartridge_load("Test ROMs/mem_timing/individual/03-modify_timing");

    /**** APU test ROM ****/
    //cartridge_load("Test ROMs/dmg_sound/rom_singles/01-registers");

    /**** PPU test ROM ****/
    //cartridge_load("Test ROMs/PPU/dmg-acid2");   // OK

    //cartridge_load("Tetris");
    //cartridge_load("Dr. Mario");
    //cartridge_load("Kirby's Dream Land");
    //cartridge_load("Super Mario Land");
    //cartridge_load("DuckTales");
    //cartridge_load("Tennis");
    //cartridge_load("Bubble Bobble");
    //cartridge_load("F-1 Race");    
    //cartridge_load("Super Mario Land 2 - 6 Golden Coins");
    //cartridge_load("World Cup 98");
    cartridge_load("Legend of Zelda, The - Link's Awakening");

    /**** initialize emulator's systems ****/
    CPU_init(&cpu);
    PPU_init();
    APU_init();
    timer_init();

    /**** emulation loop ****/
    int running = 1;
    while (running)  
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = 0;

        clock();
    }
    
    APU_deinit();
    PPU_deinit();

    SDL_Quit();

    return 0;
}

void clock(void)
{
    if (DMA_active)
        DMA_copy();

    CPU_execute_machine_cycle(&cpu);

    timer_clock();

    PPU_clock();
    PPU_clock();
    PPU_clock();
    PPU_clock();

    APU_clock();
    APU_clock();
    APU_clock();
    APU_clock();
}