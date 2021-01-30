#include <stdio.h>
#include "CPU.h"
#include "instruction_set.h"
#include "bus.h"

CPU cpu;

int CPU_init(CPU *cpu)
{
    // load bootstrap ROM 
    FILE *bootROM = fopen("ROMs/Nintendo Game Boy Boot ROM.gb", "rb");
    if (!bootROM)
    {
        printf("error opening boot ROM");
        return 0;
    }

    size_t elements_read = fread(cpu->bootROM, 1, 0x100, bootROM);
    if (elements_read < 0x100)
    {
        printf("error reading boot ROM");
        return 0;
    }

    CPU_Reset(cpu);

    return 1;
}

void CPU_Reset(CPU *cpu)
{
    //cpu->PC = 0x0000;
    cpu->PC = 0x0100;

    cpu->A = 0x01;
    cpu->F.reg = 0xB0;

    cpu->B = 0x00;
    cpu->C = 0x13;
    cpu->D = 0x00;
    cpu->E = 0xD8;
    cpu->H = 0x01;
    cpu->L = 0x4D;

    cpu->SP = 0xFFFE;

    //cpu->boot = 1;  // enable bootstrap ROM
    cpu->boot = 0;    // disable bootstrap ROM

    cpu->IME = 0;
    cpu->EI = 0; 
    cpu->halt_mode = 0;

    cpu->total_machine_cycles = 0;

    cpu->current_machine_cycle = 1;  

    cpu->address_bus = cpu->PC;                             
    cpu->PC++;                                                 
    cpu->data_bus = bus_read(cpu->address_bus);
    cpu->instruction_register = cpu->data_bus;                 

    cpu->current_instruction = &instruction_table[cpu->instruction_register];
}

#define INT_VECTOR_VBLANK    0x0040
#define INT_VECTOR_LCD       0x0048
#define INT_VECTOR_TIMER     0x0050
#define INT_VECTOR_SERIAL    0x0058
#define INT_VECTOR_JOYPAD    0x0060

int CPU_check_interrupts(CPU *cpu)
{
	if (cpu->EI)  // last instruction was EI - interrupts enabled after next instruction
	{
		cpu->IME = 1;
		cpu->EI = 0;

		return 0;
	}
 
    // handle interrupt if Interrupt Master Enable flag is 1
    // IME enables jump to interrupt vectors
    // is set by EI and RETI instructions and reset by DI or by CPU jumping to interupt vector

    if (cpu->IME)
    {
        if (check_int_enabled(INT_VBLANK) && check_int_flag(INT_VBLANK))  // VBLANK
        {
            clear_int_flag(INT_VBLANK);        // acknowledge interrupt

            cpu->interrupt_vector = INT_VECTOR_VBLANK;

            cpu->current_machine_cycle = 1;
            cpu->current_instruction = &interrupt;

            return 1;
        }
        else if (check_int_enabled(INT_LCD_STAT) && check_int_flag(INT_LCD_STAT))  // LCD STAT
        {
            clear_int_flag(INT_LCD_STAT);
  
            cpu->interrupt_vector = INT_VECTOR_LCD;

            cpu->current_machine_cycle = 1;
            cpu->current_instruction = &interrupt;

            return 1;
        }
        else if (check_int_enabled(INT_TIMER) && check_int_flag(INT_TIMER))   // timer
        {
            clear_int_flag(INT_TIMER);

            cpu->interrupt_vector = INT_VECTOR_TIMER;
            
            cpu->current_machine_cycle = 1;
            cpu->current_instruction = &interrupt;

            return 1;
        }
        else if (check_int_enabled(INT_SERIAL) && check_int_flag(INT_SERIAL))  // serial
        {
            clear_int_flag(INT_SERIAL);

            cpu->interrupt_vector = INT_VECTOR_SERIAL;
            
            cpu->current_machine_cycle = 1;
            cpu->current_instruction = &interrupt;

            return 1;
        }
        else if (check_int_enabled(INT_JOYPAD) && check_int_flag(INT_JOYPAD))   // joypad 
        {
            clear_int_flag(INT_JOYPAD);

            cpu->interrupt_vector = INT_VECTOR_JOYPAD;
            
            cpu->current_machine_cycle = 1;
            cpu->current_instruction = &interrupt;

            return 1;
        }
    }

    return 0;
}

void CPU_execute_machine_cycle(CPU *cpu)
{
    if (cpu->halt_mode)
    {
        uint8_t IE = bus_read(INT_ENABLE_REG), IF = bus_read(INT_FLAG_REG);
        uint8_t pending = IE & IF & INT_REG_MASK;

        if (pending)
        {
            cpu->halt_mode = 0;
            cpu->current_instruction->instruction_handler = &NOP;
            cpu->current_instruction->machine_cycles = 1;  // exiting halt mode takes 1 machine cycles

            cpu->current_machine_cycle = 1;
        }
        else
            return;
    }
    else if (cpu->current_machine_cycle > cpu->current_instruction->machine_cycles)  // if instruction execution ended fetch new opcode
    {
        // check and service interrupts after each instruction
        if (!CPU_check_interrupts(cpu))  // if interrupt is not being serviced
        {
            cpu->current_machine_cycle = 1;  // start from machine cycle M1

            // common first three clock cycles (T cycles) of first machine cycle M1: instruction fetch
            cpu->address_bus = cpu->PC;                                
            cpu->PC++;                                                
            cpu->data_bus = bus_read(cpu->address_bus);
            cpu->instruction_register = cpu->data_bus;                

            if (cpu->instruction_register == 0xCB)   // fetch extended opcode
            {
                cpu->address_bus = cpu->PC;
                cpu->PC++;
                cpu->data_bus = bus_read(cpu->address_bus);
                cpu->instruction_register = cpu->data_bus;

                cpu->current_instruction = &extended_instruction_table[cpu->instruction_register];  // "decode" extended instruction    
            }
            else
                cpu->current_instruction = &instruction_table[cpu->instruction_register];           // "decode" instruction   

            //CPU_log(cpu);
        }
    }

    cpu->current_instruction->instruction_handler(cpu);    // execute current instruction's machine (M) cycle 
    cpu->current_machine_cycle++;
}

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void CPU_log(CPU *cpu)
{
    char *p = NULL;
    char temp[40], string[40];

    if (p = strstr(cpu->current_instruction->name, "u8"))
    {
        size_t len = strlen(cpu->current_instruction->name) - strlen(p);
        strncpy(string, cpu->current_instruction->name, len);
        string[len] = '\0';

        uint8_t immediate8 = bus_read(cpu->PC);
        char intstr[3]; // 2 chars + null char
        _itoa(immediate8, intstr, 16);

        strcat(string, "0x");
        strcat(string, _strupr(intstr));

        strcat(string, p + 2);  // skip "u8"
       
        printf("0x%04X: %-20s\n", cpu->PC - 1, string);
    }
    else if (p = strstr(cpu->current_instruction->name, "u16"))
    {
        size_t len = strlen(cpu->current_instruction->name) - strlen(p);
        strncpy(string, cpu->current_instruction->name, len);
        string[len] = '\0';

        uint16_t immediate16 = bus_read(cpu->PC);
        immediate16 |= bus_read(cpu->PC + 1) << 8;
        char intstr[5]; // 4 chars + null char
        _itoa(immediate16, intstr, 16);

        strcat(string, "0x");                     
        strcat(string, _strupr(intstr));

        strcat(string, p + 3);  // skip "u16"

        printf("0x%04X: %-20s\n", cpu->PC - 1, string);
    }
    else if (p = strstr(cpu->current_instruction->name, "i8"))
    {
        size_t len = strlen(cpu->current_instruction->name) - strlen(p);
        strncpy(string, cpu->current_instruction->name, len);
        string[len] = '\0';

        int8_t signed_immediate8 = bus_read(cpu->PC);
        char intstr[5]; // 4 chars + null char - 3 chars + sign + null char
        _itoa(cpu->PC - 1 + signed_immediate8 + 2, intstr, 16);
        strcat(string, "0x");
        strcat(string, _strupr(intstr));
        strcat(string, " (");
        if (signed_immediate8 >= 0)
            strcat(string, "+");
        _itoa(signed_immediate8 + 2, intstr, 10);
        strcat(string, intstr);
        strcat(string, ")");

        strcat(string, p + 2);  // skip "i8"

        printf("0x%04X: %-20s\n", cpu->PC - 1, string);
    }
    else
        printf("0x%04X: %-20s\n", cpu->PC - 1, cpu->current_instruction->name);

    printf("SP: 0x%04X\n", cpu->SP);
    printf("Z: %u - N: %u - H: %u - C: %u\n", cpu->F.bits.Z, cpu->F.bits.N, cpu->F.bits.H, cpu->F.bits.C);
    printf("A: 0x%02X F: 0x%02X\n", cpu->A, cpu->F.reg & 0xF0);
    printf("B: 0x%02X C: 0x%02X\n", cpu->B, cpu->C);
    printf("D: 0x%02X E: 0x%02X\n", cpu->D, cpu->E);
    printf("H: 0x%02X L: 0x%02X\n", cpu->H, cpu->L);
    printf("IME: %d\n", cpu->IME);
    printf("\n");
}