#include <stdio.h>
#include "CPU.h"
#include "instruction_set.h"
#include "bus.h"

CPU cpu;

int CPU_init(CPU *cpu)
{
    // load bootstrap ROM 
    cpu->boot = 1;
    //cpu->boot = 0;

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
    cpu->PC = 0x0000;
    //cpu->PC = 0x0100;

    cpu->current_machine_cycle = 1;  

    cpu->address_bus = cpu->PC;                             
    cpu->PC++;                                                 
    cpu->data_bus = bus_read(cpu->address_bus);
    cpu->instruction_register = cpu->data_bus;                 

    cpu->current_instruction = &instruction_table[cpu->instruction_register];

    CPU_log(cpu);
}

void CPU_BUSRQ(CPU *cpu)
{

}

void CPU_NMI(CPU *cpu)
{

}

void CPU_INT(CPU *cpu)
{

}

void CPU_execute_machine_cycle(CPU *cpu)
{
    if (cpu->current_machine_cycle > cpu->current_instruction->machine_cycles)  // if instruction execution ended fetch new opcode
    {
        cpu->current_machine_cycle = 1;  // start from machine cycle M1

        // common first three clock cycles (T cycles) of first machine cycle M1: instruction fetch
        cpu->address_bus = cpu->PC;                                // T1
        cpu->PC++;                                                 // T2
        cpu->data_bus = bus_read(cpu->address_bus);     
        cpu->instruction_register = cpu->data_bus;                 // T3

        if (cpu->instruction_register == 0xCB)  // fetch extended opcode
        {
            cpu->address_bus = cpu->PC;                                // T1
            cpu->PC++;                                                 // T2
            cpu->data_bus = bus_read(cpu->address_bus);     
            cpu->instruction_register = cpu->data_bus;                 // T3

            cpu->current_instruction = &extended_instruction_table[cpu->instruction_register];  // "decode" extended instruction    
        }
        else
            cpu->current_instruction = &instruction_table[cpu->instruction_register];           // "decode" instruction    

        CPU_log(cpu);
    }

    cpu->current_instruction->instruction_handler(cpu);           // execute current instruction's machine (M) cycle 
    cpu->current_machine_cycle++;            

    // interrupt handling                   
}

#include <string.h>
#include <stdlib.h>

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
        strcat(string, intstr);

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
        strcat(string, intstr);

        strcat(string, p + 3);  // skip "u16"

        printf("0x%04X: %-20s\n", cpu->PC - 1, string);
    }
    else if (p = strstr(cpu->current_instruction->name, "i8"))
    {
        size_t len = strlen(cpu->current_instruction->name) - strlen(p);
        strncpy(string, cpu->current_instruction->name, len);
        string[len] = '\0';

        int8_t signed_immediate8 = bus_read(cpu->PC);
        char intstr[5]; // 3 chars + sign + null char
        _itoa(signed_immediate8, intstr, 10);

        strcat(string, intstr);

        strcat(string, p + 2);  // skip "i8"

        printf("0x%04X: %-20s\n", cpu->PC - 1, string);
    }
    else
        printf("0x%04X: %-20s\n", cpu->PC - 1, cpu->current_instruction->name);

    printf("SP: 0x%04X\n", cpu->SP);
    printf("Z: %u - N: %u - H: %u - C: %u\n", cpu->F.bits.Z, cpu->F.bits.N, cpu->F.bits.H, cpu->F.bits.C);
    printf("A: 0x%02X", cpu->A);
    printf("B: 0x%02X C: 0x%02X\n", cpu->C, cpu->C);
    printf("D: 0x%02X E: 0x%02X\n", cpu->D, cpu->E);
    printf("H: 0x%02X L: 0x%02X\n", cpu->H, cpu->L);
    printf("\n");
}