#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>

typedef struct CPU
{
    //data bus
    uint8_t data_bus;

    // address bus
    uint16_t address_bus;

    // accumulator
    uint8_t A;

    // flag/status register
    union FlagRegister
    {
        struct
        {
            uint8_t unused : 4; // little-endian machine      
            uint8_t C : 1;      // carry flag (carry or borrow/ninth shift-rotation bit)
            uint8_t H : 1;      // half-carry flag
            uint8_t N : 1;      // add/subtract flag
            uint8_t Z : 1;      // zero flag
        } bits;  

        uint8_t reg;
    } F;

    // general purpose registers
    uint8_t B;
    uint8_t C;
    
    uint8_t D;
    uint8_t E;
    
    uint8_t H;
    uint8_t L;

    // stack pointer (points to top of stack - full descending stack)
    uint16_t SP;

    // program counter/instruction pointer
    uint16_t PC; 

    // shadow registers (not exposed to the programmer)
    uint8_t W;
    uint8_t Z;
    uint8_t tmp;

    // instruction decoder
    uint8_t instruction_register;              // current opcode
    struct Instruction *current_instruction;   // instruction being executed
    uint8_t current_machine_cycle;             // executing instruction's current machine cycle
    
    int halt_mode;

    int EI;   // EI instruction executed, interrupts enabled after the following instruction
    int IME;  // Interrupt Master Enable flag

    uint8_t bootROM[0x100];   // bootstrap ROM 0x00 - 0xFF
    int boot;                 // bootstrap ROM enabled

    uint16_t interrupt_vector;

    uint64_t total_machine_cycles;
    
} CPU;

extern CPU cpu;

int CPU_init(CPU *cpu);
void CPU_Reset(CPU *cpu);
void CPU_execute_machine_cycle(CPU *cpu);
int CPU_check_interrupts(CPU *cpu);    // sensed at end of instruction
void CPU_log(CPU *cpu);

#endif  // __CPU_H__


