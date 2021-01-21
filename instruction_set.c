#include "instruction_set.h"
#include "CPU.h"
#include "bus.h"

/**** 8-bit Load Commands ****/

// LD r, r'
void LD_r1_r2(CPU *cpu)
{
    switch (cpu->instruction_register & 0x07u)   
    {
        case 0:
            cpu->tmp = cpu->B;
            break;
        case 1:
            cpu->tmp = cpu->C;
            break;
        case 2:
            cpu->tmp = cpu->D;
            break;
        case 3:
            cpu->tmp = cpu->E;
            break;
        case 4:
            cpu->tmp = cpu->H;
            break;
        case 5:
            cpu->tmp = cpu->L;
            break;
        case 7:
            cpu->tmp = cpu->A;
            break;
    }

    switch (cpu->instruction_register >> 3u & 0x07u)   
    {
        case 0:
            cpu->B = cpu->tmp;
            break;
        case 1:
            cpu->C = cpu->tmp;
            break;
        case 2:
            cpu->D = cpu->tmp;
            break;
        case 3:
            cpu->E = cpu->tmp;
            break;
        case 4:
            cpu->H = cpu->tmp;
            break;
        case 5:
            cpu->L = cpu->tmp;
            break;
        case 7:
            cpu->A = cpu->tmp;
            break;
    }
}

// LD r, u8
void LD_r_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:  
            break;
        case 2: 
            cpu->address_bus = cpu->PC;                                    
            cpu->PC++;    
            cpu->data_bus = bus_read(cpu->address_bus);

            switch (cpu->instruction_register >> 3u & 0x07u)  
            {
                case 0:
                    cpu->B = cpu->data_bus;
                    break;
                case 1:
                    cpu->C = cpu->data_bus;
                    break;
                case 2:
                    cpu->D = cpu->data_bus;
                    break;
                case 3:
                    cpu->E = cpu->data_bus;
                    break;
                case 4:
                    cpu->H = cpu->data_bus;
                   break;
                case 5:
                    cpu->L = cpu->data_bus;
                    break;
                case 7:
                    cpu->A = cpu->data_bus;
                    break;
            }

            break;
    }
}

// LD r, (HL)
void LD_r_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:  
            break;
        case 2:  
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L; 
            cpu->data_bus = bus_read(cpu->address_bus);

            switch (cpu->instruction_register >> 3u & 0x07u)  
            {
                case 0:
                    cpu->B = cpu->data_bus;
                    break;
                case 1:
                    cpu->C = cpu->data_bus;
                    break;
                case 2:
                    cpu->D = cpu->data_bus;
                    break;
                case 3:
                    cpu->E = cpu->data_bus;
                    break;
                case 4:
                    cpu->H = cpu->data_bus;
                   break;
                case 5:
                    cpu->L = cpu->data_bus;
                    break;
                case 7:
                    cpu->A = cpu->data_bus;
                    break;
            }
                         
            break;
    }
}

// LD (HL), r
void LD_ind_HL_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1: 
            break;
        case 2: 
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L; 

            switch (cpu->instruction_register & 0x07u) 
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }

            bus_write(cpu->address_bus, cpu->tmp);          
            break;
    }
}

// LD (HL), u8 
void LD_ind_HL_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:  
            break;
        case 2:  
            cpu->address_bus = cpu->PC;                           
            cpu->PC++;                                            
            cpu->Z = bus_read(cpu->address_bus);            
            break;
        case 3:  
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L;    
            bus_write(cpu->address_bus, cpu->Z);           
            break;
    }
}

// LD A, (BC)
void LD_A_ind_BC(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->B << 8u | cpu->C;
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LD A, (DE)
void LD_A_ind_DE(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->D << 8u | cpu->E;
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LD (BC), A
void LD_ind_BC_A(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->B << 8u | cpu->C;    
            bus_write(cpu->address_bus, cpu->A);              
            break;
    }
}

// LD (DE), A
void LD_ind_DE_A(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->D << 8u | cpu->E;   
            bus_write(cpu->address_bus, cpu->A);              
            break;
    }
}

// LDI (HL), A -- LD (HLI), A -- LD (HL+), A
void LDI_ind_HL_A(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L;
            cpu->L++;
            if (cpu->L == 0x00)
                cpu->H++;
            bus_write(cpu->address_bus, cpu->A);
            break;
    }
}

// LDI A, (HL) -- LD A, (HLI) -- LD A, (HL+)
void LDI_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L;
            cpu->L++;
            if (cpu->L == 0x00)
                cpu->H++;
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LDD (HL), A -- LD (HLD), A -- LD (HL-), A
void LDD_ind_HL_A(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L;
            cpu->L--;
            if (cpu->L == 0xFF)
                cpu->H--;
            bus_write(cpu->address_bus, cpu->A);
            break;
    }
}

// LDD A, (HL) -- LD A, (HLD) -- LD A, (HL-)
void LDD_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L;
            cpu->L--;
            if (cpu->L == 0x00)
                cpu->H--;
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LD A, (u16) 
void LD_A_ind_u16(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:  
            break;
        case 2:  
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3: 
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:  
            cpu->address_bus = (uint16_t)cpu->W << 8u | cpu->Z; 
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LD (u16), A 
void LD_ind_u16_A(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->address_bus = (uint16_t)cpu->W << 8u | cpu->Z;
            bus_write(cpu->address_bus, cpu->A);
            break;
    }
}

// LD A, ($FF00 + u8)
void LD_A_ind_FF00_plus_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = 0xFF00 | cpu->Z;
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LD ($FF00 + u8), A
void LD_ind_FF00_plus_u8_A(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = 0xFF00 | cpu->Z;
            bus_write(cpu->address_bus, cpu->A);
            break;
    }
}

// LD A, ($FF00 + C)
void LD_A_ind_FF00_plus_C(CPU *cpu)
{
    switch(cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = 0xFF00 | cpu->C;
            cpu->A = bus_read(cpu->address_bus);
            break;
    }
}

// LD ($FF00 + C), A
void LD_ind_FF00_plus_C_A(CPU *cpu)
{
    switch(cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = 0xFF00 | cpu->C;
            bus_write(cpu->address_bus, cpu->A);
            break;
    }
}

/**** 16-bit Load Commands ****/

// LD rr, u16
void LD_rr_u16(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;

            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->C = bus_read(cpu->address_bus);
                    break;
                case 1:
                    cpu->E = bus_read(cpu->address_bus);
                    break;
                case 2:
                    cpu->L = bus_read(cpu->address_bus);
                    break;
                case 3:
                    cpu->SP = cpu->SP & 0xFF00 | bus_read(cpu->address_bus);
                    break;
            }

            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;

            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->B = bus_read(cpu->address_bus);
                    break;
                case 1:
                    cpu->D = bus_read(cpu->address_bus);
                    break;
                case 2:
                    cpu->H = bus_read(cpu->address_bus);
                    break;
                case 3:
                    cpu->SP = cpu->SP & 0x00FF | (uint16_t)bus_read(cpu->address_bus) << 8u;
                    break;
            }

            break;
    }
}

// LD (u16), SP
void LD_ind_u16_SP(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->address_bus = (uint16_t)cpu->W << 8u | cpu->Z;
            bus_write(cpu->address_bus, cpu->SP & 0x00FF);
            cpu->Z++;
            if (cpu->Z == 0x00)
                cpu->W++;
            break;
        case 5:
            cpu->address_bus = (uint16_t)cpu->W << 8u | cpu->Z;
            bus_write(cpu->address_bus, cpu->SP >> 8u & 0x00FF);
            break;
    }
}

// LD SP, HL
void LD_SP_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->SP = (uint16_t)cpu->H << 8u | cpu->L;
            break;
    }
}

// LD HL, SP + i8
void LD_HL_SP_plus_i8(CPU *cpu) 
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            //cpu->SP & 0x00FF;
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            uint16_t result = (cpu->SP & 0x00FF) + cpu->tmp;
            cpu->F.bits.H = result >> 8u;
            cpu->F.bits.Z = 0;
            cpu->F.bits.N = 0;
            cpu->L = result & 0x00FF;
            break;
        case 3:
            result = (cpu->SP >> 8u & 0x00FF) + cpu->F.bits.H;
            cpu->F.bits.C = result >> 8u;
            cpu->H = result;
            break;
    }  
}

// PUSH BC / PUSH DE / PUSH HL / PUSH AF
void PUSH_rr(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->SP;
            cpu->SP--;     
            break;
        case 3:
            cpu->address_bus = cpu->SP;
            
            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->data_bus = cpu->B;
                    break;
                case 1:
                    cpu->data_bus = cpu->D;
                    break;
                case 2:
                    cpu->data_bus = cpu->H;
                    break;
                case 3:
                    cpu->data_bus = cpu->A;
                    break;
            }

            bus_write(cpu->address_bus, cpu->data_bus);
            cpu->SP--;
            break;
        case 4:
            cpu->address_bus = cpu->SP;
            
            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->data_bus = cpu->C;
                    break;
                case 1:
                    cpu->data_bus = cpu->E;
                    break;
                case 2:
                    cpu->data_bus = cpu->L;
                    break;
                case 3:
                    cpu->data_bus = cpu->F.reg;
                    break;
            }

            bus_write(cpu->address_bus, cpu->data_bus);
            break;
    }
}

// POP BC / POP DE / POP HL / POP AF
void POP_rr(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->data_bus = bus_read(cpu->address_bus);

            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->C = cpu->data_bus;
                    break;
                case 1:
                    cpu->E = cpu->data_bus;
                    break;
                case 2:
                    cpu->L = cpu->data_bus;
                    break;
                case 3:
                    cpu->F.reg = cpu->data_bus;  
                    break;
            }

            break;
        case 3:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->data_bus = bus_read(cpu->address_bus);

            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->B = cpu->data_bus;
                    break;
                case 1:
                    cpu->D = cpu->data_bus;
                    break;
                case 2:
                    cpu->H = cpu->data_bus;
                    break;
                case 3:
                    cpu->A = cpu->data_bus;  
                    break;
            }

            break;
    }
}

/**** 8-bit Arithmetic/logical Commands ****/

// ADD A, B / ADD A, C / ADD A, D / ADD A, E / ADD A, H / ADD A, L / ADD A, A
void ADD_A_r(CPU *cpu)
{   
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07u)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }
            
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A + cpu->tmp;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA + lowNibbleTmp;
            uint8_t halfCarry = lowNibble >> 4u & 0x0F;

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA + highNibbleTmp + halfCarry;
            uint8_t carry = highNibble >> 4u & 0x0F;

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// ADD A, (HL)
void ADD_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8u | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);

            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A + cpu->tmp;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA + lowNibbleTmp;
            uint8_t halfCarry = lowNibble >> 4u & 0x0F;

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA + highNibbleTmp + halfCarry;
            uint8_t carry = highNibble >> 4u & 0x0F;

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// ADC A, B / ADC A, C / ADC A, D / ADC A, E / ADC A, H / ADC A, L / ADC A, A
void ADC_A_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A + cpu->tmp + cpu->F.bits.C;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA + lowNibbleTmp;
            uint8_t halfCarry = lowNibble >> 4u & 0x0F;

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA + highNibbleTmp + halfCarry;
            uint8_t carry = highNibble >> 4u & 0x0F;

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// ADC A, (HL)
void ADC_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A + cpu->tmp + cpu->F.bits.C;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA + lowNibbleTmp;
            uint8_t halfCarry = lowNibble >> 4u & 0x0F;

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA + highNibbleTmp + halfCarry;
            uint8_t carry = highNibble >> 4u & 0x0F;

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// ADD A, u8
void ADD_A_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A + cpu->tmp;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA + lowNibbleTmp;
            uint8_t halfCarry = lowNibble >> 4u & 0x0F;

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA + highNibbleTmp + halfCarry;
            uint8_t carry = highNibble >> 4u & 0x0F;

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// ADC A, u8
void ADC_A_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A + cpu->tmp + cpu->F.bits.C;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA + lowNibbleTmp;
            uint8_t halfCarry = lowNibble >> 4u & 0x0F;

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA + highNibbleTmp + halfCarry;
            uint8_t carry = highNibble >> 4u & 0x0F;

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// SUB A, B / SUB A, C / SUB A, D / SUB A, E / SUB A, H / SUB A, L / SUB A, A
void SUB_A_r(CPU *cpu)    // TODO:: check carry and half carry implementation!!!!
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A - cpu->tmp;

            // update flags
            int8_t lowNibbleA = oldA & 0x0F;  
            int8_t lowNibbleTmp = cpu->tmp & 0x0F;
            int8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble < 0;  // H set if there's a borrow from bit4

            int8_t highNibbleA = oldA >> 4u & 0x0F;
            int8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            int8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < 0;    // C set if there's a borrow 

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// SUB A, (HL)
void SUB_A_ind_HL(CPU *cpu)     // TODO:: check carry and half carry implementation!!!!
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A - cpu->tmp;

            // update flags
            int8_t lowNibbleA = oldA & 0x0F;  
            int8_t lowNibbleTmp = cpu->tmp & 0x0F;
            int8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble < 0;  // H set if there's a borrow from bit4

            int8_t highNibbleA = oldA >> 4u & 0x0F;
            int8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            int8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < 0;    // C set if there's a borrow 

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// SUBC A, B / SUBC A, C / SUBC A, D / SUBC A, E / SUBC A, H / SUBC A, L / SUBC A, A
void SBC_A_r(CPU *cpu)     // TODO:: check carry and half carry implementation!!!!
{ 
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A - cpu->tmp - cpu->F.bits.C;

            // update flags
            int8_t lowNibbleA = oldA & 0x0F;  
            int8_t lowNibbleTmp = cpu->tmp & 0x0F;
            int8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble < 0;  // H set if there's a borrow from bit4

            int8_t highNibbleA = oldA >> 4u & 0x0F;
            int8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            int8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < 0;    // C set if there's a borrow 

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// SBC A, (HL)
void SBC_A_ind_HL(CPU *cpu)     // TODO:: check carry and half carry implementation!!!!
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A - cpu->tmp - cpu->F.bits.C;

            // update flags
            int8_t lowNibbleA = oldA & 0x0F;  
            int8_t lowNibbleTmp = cpu->tmp & 0x0F;
            int8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble < 0;  // H set if there's a borrow from bit4

            int8_t highNibbleA = oldA >> 4u & 0x0F;
            int8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            int8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < 0;    // C set if there's a borrow 

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// SUB A, u8
void SUB_A_u8(CPU *cpu)    // TODO:: check carry and half carry implementation!!!!
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A - cpu->tmp;

            // update flags
            int8_t lowNibbleA = oldA & 0x0F;  
            int8_t lowNibbleTmp = cpu->tmp & 0x0F;
            int8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble < 0;  // H set if there's a borrow from bit4

            int8_t highNibbleA = oldA >> 4u & 0x0F;
            int8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            int8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < 0;    // C set if there's a borrow 

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// SBC A, u8
void SBC_A_u8(CPU *cpu)     // TODO:: check carry and half carry implementation!!!!
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            
            uint8_t oldA = cpu->A;   // save A for flag test
            cpu->A = cpu->A - cpu->tmp - cpu->F.bits.C;

            // update flags
            uint8_t lowNibbleA = oldA & 0x0F;  
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble > lowNibbleA;  // H set if there's a borrow from bit4

            uint8_t highNibbleA = oldA >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < highNibbleA;    // C set if there's a borrow 

            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// AND A, B / AND A, C / AND A, D / AND A, E / AND A, H / AND A, L / AND A, A
void AND_A_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07u)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }

            cpu->A = cpu->A & cpu->tmp;

            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 1;
            cpu->F.bits.C = 0;

            break;
    }
}

// AND A, (HL)
void AND_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->A = cpu->A & cpu->tmp;

            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 1;
            cpu->F.bits.C = 0;

            break;
    }
}

// AND A, u8
void AND_A_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->A = cpu->A & cpu->tmp;
            
            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 1;
            cpu->F.bits.C = 0;

            break;
    }
}

// XOR A, B / XOR A, C / XOR A, D / XOR A, E / XOR A, H / XOR A, L / XOR A, A
void XOR_A_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }

            cpu->A = cpu->A ^ cpu->tmp;

            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;

            break;
    }
}

// XOR A, (HL)
void XOR_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->A = cpu->A ^ cpu->tmp;

            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 1;
            cpu->F.bits.C = 0;
            
            break;
    }
}

// XOR A, u8
void XOR_A_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->A = cpu->A ^ cpu->tmp;
            
            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;

            break;
    }
}

// OR A, B / OR A, C / OR A, D / OR A, E / OR A, H / OR A, L / OR A, A
void OR_A_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }

            cpu->A = cpu->A | cpu->tmp;

            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;

            break;
    }
}

// OR A, (HL)
void OR_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->A = cpu->A | cpu->tmp;

            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;

            break;
    }
}

// OR A, u8
void OR_A_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->A = cpu->A | cpu->tmp;
            
            // update flags
            cpu->A == 0x00 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;
            
            break;
    }
}

// CP A, r
void CP_A_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break; 
            }

            // update flags
            int comparison = cpu->A == cpu->tmp;
            
            uint8_t lowNibbleA = cpu->A & 0x0F;  
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble > lowNibbleA;  // H set if there's a borrow from bit4

            uint8_t highNibbleA = cpu->A >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < highNibbleA;    // C set if there's a borrow 

            cpu->F.bits.Z = comparison;
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// CP A, (HL)
void CP_A_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            
            // update flags
            int comparison = cpu->A == cpu->tmp;
            
            uint8_t lowNibbleA = cpu->A & 0x0F;  
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble > lowNibbleA;  // H set if there's a borrow from bit4

            uint8_t highNibbleA = cpu->A >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < highNibbleA;    // C set if there's a borrow 

            cpu->F.bits.Z = comparison;
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// CP A, u8
void CP_A_u8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);
            
            // update flags
            int comparison = cpu->A == cpu->tmp;
            
            uint8_t lowNibbleA = cpu->A & 0x0F;  
            uint8_t lowNibbleTmp = cpu->tmp & 0x0F;
            uint8_t lowNibble = lowNibbleA - lowNibbleTmp;
            uint8_t halfCarry = lowNibble > lowNibbleA;  // H set if there's a borrow from bit4

            uint8_t highNibbleA = cpu->A >> 4u & 0x0F;
            uint8_t highNibbleTmp = cpu->tmp >> 4u & 0x0F;
            uint8_t highNibble = highNibbleA - highNibbleTmp - halfCarry;
            uint8_t carry = highNibble < highNibbleA;    // C set if there's a borrow 

            cpu->F.bits.Z = comparison;
            cpu->F.bits.N = 1;
            halfCarry ? (cpu->F.bits.H = 1) : (cpu->F.bits.H = 0);
            carry ? (cpu->F.bits.C = 1) : (cpu->F.bits.C = 0);

            break;
    }
}

// INC r
void INC_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
        {
            uint8_t before, after;

            switch (cpu->instruction_register >> 3u & 0x07u)
            {
                case 0:
                    before = cpu->B;
                    after = cpu->B++;
                    break;
                case 1:
                    before = cpu->C;    
                    after = cpu->C++;
                    break;
                case 2:
                    before = cpu->D;
                    after = cpu->D++;
                    break;
                case 3:
                    before = cpu->E;
                    after = cpu->E++;
                    break;
                case 4:
                    before = cpu->H;
                    after = cpu->H++;
                    break;
                case 5:
                    before = cpu->L;
                    after = cpu->L++;
                    break;
                case 7:
                    before = cpu->A;
                    after = cpu->A++;
                    break; 
            }

            // update flags
            after == 0 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->H = (before & 0x0Fu) + 1u & 0x10u;           
            // C flag not affected

            break;
        }
    }
}

// INC (HL)
void INC_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            
            cpu->tmp++;

            // update flags
            cpu->tmp == 0 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->H = (cpu->tmp - 1u & 0x0Fu) + 1u & 0x10u;           
            // C flag not affected

            break;
        case 3:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

// DEC r
void DEC_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
        {
            uint8_t before, after;

            switch (cpu->instruction_register >> 3u & 0x07u)
            {
                case 0:
                    before = cpu->B;
                    after = cpu->B--;
                    break;
                case 1:
                    before = cpu->C;    
                    after = cpu->C--;
                    break;
                case 2:
                    before = cpu->D;
                    after = cpu->D--;
                    break;
                case 3:
                    before = cpu->E;
                    after = cpu->E--;
                    break;
                case 4:
                    before = cpu->H;
                    after = cpu->H--;
                    break;
                case 5:
                    before = cpu->L;
                    after = cpu->L--;
                    break;
                case 7:
                    before = cpu->A;
                    after = cpu->A--;
                    break; 
            }

            // update flags
            after == 0 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->H = (before & 0x0Fu) - 1u & 0x10u;           
            // C flag not affected

            break;
        }
    }
}

// DEC (HL)
void DEC_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            cpu->tmp--;

            // update flags
            cpu->tmp == 0 ? (cpu->F.bits.Z = 1) : (cpu->F.bits.Z = 0);
            cpu->F.bits.N = 0;
            cpu->H = (cpu->tmp + 1u & 0x0Fu) - 1u & 0x10u;           
            // C flag not affected

            break;
        case 3:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

// DAA
void DAA(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            // TODO
            break;
    }
}

// CPL
void CPL(CPU *cpu)   
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            cpu->A = ~cpu->A;
            cpu->F.bits.N = 1;
            cpu->F.bits.H = 1;
            break;
    }
}


/****  16-bit Arithmetic/logical Commands ****/

void ADD_HL_rr(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                {
                    uint16_t result = cpu->L + cpu->C;
                    cpu->F.bits.H = result & 0x0010;
                    cpu->L = result & 0xFF;
                    result >>= 8u;
                    result = cpu->H + cpu->B + result;
                    cpu->F.bits.C = result >> 4u;
                    cpu->H = result & 0x00FF;
                    break;
                }
                case 1:
                {
                    uint16_t result = cpu->L + cpu->E;
                    cpu->F.bits.H = result & 0x0010;
                    cpu->L = result & 0xFF;
                    result >>= 8u;
                    result = cpu->H + cpu->D + result;
                    cpu->F.bits.C = result >> 4u;
                    cpu->H = result & 0x00FF;
                    break;
                }
                case 2:
                {
                    uint16_t result = cpu->L + cpu->L;
                    cpu->F.bits.H = result & 0x0010;
                    cpu->L = result & 0xFF;
                    result >>= 8u;
                    result = cpu->H + cpu->H + result;
                    cpu->F.bits.C = result >> 4u;
                    cpu->H = result & 0x00FF;
                    break;
                }
                case 3:
                {
                    uint16_t result = cpu->L + (cpu->SP & 0x00FF);
                    cpu->F.bits.H = result & 0x0010;
                    cpu->L = result & 0xFF;
                    result >>= 8u;
                    result = cpu->H + (cpu->SP >> 8u & 0x00FF) + result;
                    cpu->F.bits.C = result >> 4u;
                    cpu->H = result & 0x00FF;
                    break;
                }
            }
        
            cpu->F.bits.N = 0;

            break;
    }
}

void ADD_SP_i8(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);

            uint16_t result = (cpu->SP & 0x00FF) + cpu->tmp;
            cpu->F.bits.H = result >> 8u;

            cpu->F.bits.Z = 0;
            cpu->F.bits.N = 0;

            break;
        case 3:
            result = (cpu->SP >> 8u & 0x00FF) + cpu->F.bits.H;
            cpu->F.bits.C = result >> 8u;
            cpu->SP += cpu->tmp;
            break;
        case 4:
            break;
    }
}

// INC rr
void INC_rr(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->C++;
                    if (cpu->C == 0x00)
                        cpu->B++;
                    break;
                case 1:
                    cpu->E++;
                    if (cpu->E == 0x00)
                        cpu->D++;
                    break;
                case 2:
                    cpu->L++;
                    if (cpu->L == 0x00)
                        cpu->H++;
                    break;
                case 3:
                    cpu->SP++;
                    break;
            }       

            break;
    }
}

// DEC rr
void DEC_rr(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register >> 4u & 0x03u)
            {
                case 0:
                    cpu->C--;
                    if (cpu->C == 0xFF)
                        cpu->B--;
                    break;
                case 1:
                    cpu->E--;
                    if (cpu->E == 0xFF)
                        cpu->D--;
                    break;
                case 2:
                    cpu->L--;
                    if (cpu->L == 0xFF)
                        cpu->H--;
                    break;
                case 3:
                    cpu->SP--;
                    break;
            }
            
            break;
    }
}

/**** Rotate and Shift Commands ****/

// RLA
void RLA(CPU *cpu)
{
    uint8_t oldC = cpu->F.bits.C;        // save carry flag
    cpu->F.bits.C = cpu->A >> 7 & 0x01;  // bit7 in carry flag
    cpu->A <<= 1;                        // 0 in bit0 
    cpu->A |= oldC;                      // old carry flag (9th bit) in bit0

    cpu->F.bits.Z = 0;
    cpu->F.bits.N = 0;
    cpu->F.bits.H = 0;
}

// RLCA
void RLCA(CPU *cpu)
{
    cpu->F.bits.C = cpu->A >> 7 & 0x01;  // bit7 in carry flag
    cpu->A <<= 1;                        // 0 in bit0 
    cpu->A |= cpu->F.bits.C;             // old bit7 in bit0

    cpu->F.bits.Z = 0;
    cpu->F.bits.N = 0;
    cpu->F.bits.H = 0;
}

//RRA
void RRA(CPU *cpu)
{
    uint8_t oldC = cpu->F.bits.C;        // save carry flag
    cpu->F.bits.C = cpu->A & 0x01;       // bit0 in carry flag
    cpu->A >>= 1;                        // 0 in bit7 (unsigned int: C arithmetic shift)
    cpu->A |= oldC << 7;                 // old carry flag (9th bit) in bit7

    cpu->F.bits.Z = 0;
    cpu->F.bits.N = 0;
    cpu->F.bits.H = 0;
}

// RRCA
void RRCA(CPU *cpu)
{
    cpu->F.bits.C = cpu->A & 0x01;       // bit0 in carry flag
    cpu->A >>= 1;                        // 0 in bit7 (unsigned int: C arithmetic shift)
    cpu->A |= cpu->F.bits.C << 7;        // old bit0 in bit7

    cpu->F.bits.Z = 0;
    cpu->F.bits.N = 0;
    cpu->F.bits.H = 0;
}

// RL r
void RL_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->B >> 7 & 0x01;
                    cpu->B <<= 1;
                    cpu->B |= oldC;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 1:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->C >> 7 & 0x01;
                    cpu->C <<= 1;
                    cpu->C |= oldC;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 2:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->D >> 7 & 0x01;
                    cpu->D <<= 1;
                    cpu->D |= oldC;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 3:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->E >> 7 & 0x01;
                    cpu->E <<= 1;
                    cpu->E |= oldC;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 4:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->H >> 7 & 0x01;
                    cpu->H <<= 1;
                    cpu->H |= oldC;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 5:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->L >> 7 & 0x01;
                    cpu->L <<= 1;
                    cpu->L |= oldC;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 7: 
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->A >> 7 & 0x01;
                    cpu->A <<= 1;
                    cpu->A |= oldC;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;

            break;
    }
}

// RL (HL)
void RL_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
        {
            uint8_t oldC = cpu->F.bits.C;
            cpu->F.bits.C = cpu->tmp >> 7 & 0x01;
            cpu->tmp <<= 1;
            cpu->tmp |= oldC;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        }
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

void RLC_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->F.bits.C = cpu->B >> 7 & 0x01;
                    cpu->B <<= 1;
                    cpu->B |= cpu->F.bits.C;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 1:
                    cpu->F.bits.C = cpu->C >> 7 & 0x01;
                    cpu->C <<= 1;
                    cpu->C |= cpu->F.bits.C;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 2:
                    cpu->F.bits.C = cpu->D >> 7 & 0x01;
                    cpu->D <<= 1;
                    cpu->D |= cpu->F.bits.C;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 3:
                    cpu->F.bits.C = cpu->E >> 7 & 0x01;
                    cpu->E <<= 1;
                    cpu->E |= cpu->F.bits.C;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 4:
                    cpu->F.bits.C = cpu->H >> 7 & 0x01;
                    cpu->H <<= 1;
                    cpu->H |= cpu->F.bits.C;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 5:
                    cpu->F.bits.C = cpu->L >> 7 & 0x01;
                    cpu->L <<= 1;
                    cpu->L |= cpu->F.bits.C;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 7: 
                    cpu->F.bits.C = cpu->A >> 7 & 0x01;
                    cpu->A <<= 1;
                    cpu->A |= cpu->F.bits.C;;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;

            break;
    }
}

void RLC_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->F.bits.C = cpu->tmp >> 7 & 0x01;
            cpu->tmp <<= 1;
            cpu->tmp |= cpu->F.bits.C;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

void RR_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->B & 0x01;
                    cpu->B >>= 1;
                    cpu->B |= oldC << 7;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 1:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->C & 0x01;
                    cpu->C >>= 1;
                    cpu->C |= oldC << 7;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 2:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->D & 0x01;
                    cpu->D >>= 1;
                    cpu->D |= oldC << 7;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 3:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->E & 0x01;
                    cpu->E >>= 1;
                    cpu->E |= oldC << 7;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 4:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->H & 0x01;
                    cpu->H >>= 1;
                    cpu->H |= oldC << 7;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 5:
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->L & 0x01;
                    cpu->L >>= 1;
                    cpu->L |= oldC << 7;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 7: 
                {
                    uint8_t oldC = cpu->F.bits.C;
                    cpu->F.bits.C = cpu->A & 0x01;
                    cpu->A >>= 1;
                    cpu->A |= oldC << 7;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;

            break;
    }
}

void RR_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
        {
            uint8_t oldC = cpu->F.bits.C;
            cpu->F.bits.C = cpu->tmp & 0x01;
            cpu->tmp >>= 1;
            cpu->tmp |= oldC << 7;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        }
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

void RRC_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->F.bits.C = cpu->B & 0x01;
                    cpu->B >>= 1;
                    cpu->B |= cpu->F.bits.C << 7;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 1:
                    cpu->F.bits.C = cpu->C & 0x01;
                    cpu->C >>= 1;
                    cpu->C |= cpu->F.bits.C << 7;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 2:
                    cpu->F.bits.C = cpu->D & 0x01;
                    cpu->D >>= 1;
                    cpu->D |= cpu->F.bits.C << 7;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 3:
                    cpu->F.bits.C = cpu->E & 0x01;
                    cpu->E >>= 1;
                    cpu->E |= cpu->F.bits.C << 7;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 4:
                    cpu->F.bits.C = cpu->H & 0x01;
                    cpu->H >>= 1;
                    cpu->H |= cpu->F.bits.C << 7;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 5:
                    cpu->F.bits.C = cpu->L & 0x01;
                    cpu->L >>= 1;
                    cpu->L |= cpu->F.bits.C << 7;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 7: 
                    cpu->F.bits.C = cpu->A & 0x01;
                    cpu->A >>= 1;
                    cpu->A |= cpu->F.bits.C << 7;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;

            break;
    }
}

void RRC_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->F.bits.C = cpu->tmp & 0x01;
            cpu->tmp >>= 1;
            cpu->tmp |= cpu->F.bits.C << 7;;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

void SLA_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            break;
    }
}

void SLA_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->F.bits.C = cpu->tmp >> 7 & 0x01;
            cpu->tmp <<= 1;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;   
    }
}

void SRA_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                {
                    cpu->F.bits.C = cpu->B & 0x01;
                    uint8_t bit7 = cpu->B & 0x80;
                    cpu->B >>= 1;
                    cpu->B |= bit7;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 1:
                {
                    cpu->F.bits.C = cpu->C & 0x01;
                    uint8_t bit7 = cpu->C & 0x80;
                    cpu->C >>= 1;
                    cpu->C |= bit7;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 2:
                {
                    cpu->F.bits.C = cpu->D & 0x01;
                    uint8_t bit7 = cpu->D & 0x80;
                    cpu->D >>= 1;
                    cpu->D |= bit7;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 3:
                {
                    cpu->F.bits.C = cpu->E & 0x01;
                    uint8_t bit7 = cpu->E & 0x80;
                    cpu->E >>= 1;
                    cpu->E |= bit7;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 4:
                {
                    cpu->F.bits.C = cpu->H & 0x01;
                    uint8_t bit7 = cpu->H & 0x80;
                    cpu->H >>= 1;
                    cpu->H |= bit7;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 5:
                {
                    cpu->F.bits.C = cpu->L & 0x01;
                    uint8_t bit7 = cpu->L & 0x80;
                    cpu->L >>= 1;
                    cpu->L |= bit7;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 7: 
                {
                    cpu->F.bits.C = cpu->A & 0x01;
                    uint8_t bit7 = cpu->A & 0x80;
                    cpu->A >>= 1;
                    cpu->A |= bit7;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;

            break;
    }
}

void SRA_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->F.bits.C = cpu->tmp & 0x01;
            uint8_t bit7 = cpu->tmp & 0x80;
            cpu->tmp >>= 1;
            cpu->tmp |= bit7;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;   
    }
}

void SRL_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->F.bits.C = cpu->B & 0x01;
                    cpu->B >>= 1;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 1:
                    cpu->F.bits.C = cpu->C & 0x01;
                    cpu->C >>= 1;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 2:
                    cpu->F.bits.C = cpu->D & 0x01;
                    cpu->D >>= 1;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 3:
                    cpu->F.bits.C = cpu->E & 0x01;
                    cpu->E >>= 1;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 4:
                    cpu->F.bits.C = cpu->H & 0x01;
                    cpu->H >>= 1;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 5:
                    cpu->F.bits.C = cpu->L & 0x01;
                    cpu->L >>= 1;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                case 7: 
                    cpu->F.bits.C = cpu->A & 0x01;
                    cpu->A >>= 1;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;

            break;
    }
}

void SRL_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->F.bits.C = cpu->tmp & 0x01;
            cpu->tmp >>= 1;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            break;
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;   
    }
}

void SWAP_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                {
                    uint8_t lowNibble = cpu->B & 0x0F;
                    cpu->B <<= 4;
                    cpu->B |= lowNibble;
                    cpu->B == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 1:
                {
                    uint8_t lowNibble = cpu->C & 0x0F;
                    cpu->C <<= 4;
                    cpu->C |= lowNibble;
                    cpu->C == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 2:
                {
                    uint8_t lowNibble = cpu->D & 0x0F;
                    cpu->D <<= 4;
                    cpu->D |= lowNibble;
                    cpu->D == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 3:
                {
                    uint8_t lowNibble = cpu->E & 0x0F;
                    cpu->E <<= 4;
                    cpu->E |= lowNibble;
                    cpu->E == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 4:
                {
                    uint8_t lowNibble = cpu->H & 0x0F;
                    cpu->H <<= 4;
                    cpu->H |= lowNibble;
                    cpu->H == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 5:
                {
                    uint8_t lowNibble = cpu->L & 0x0F;
                    cpu->L <<= 4;
                    cpu->L |= lowNibble;
                    cpu->L == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
                case 7: 
                {
                    uint8_t lowNibble = cpu->A & 0x0F;
                    cpu->A <<= 4;
                    cpu->A |= lowNibble;
                    cpu->A == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
                    break;
                }
            }

            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;

            break;
    }
}

void SWAP_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
        {
            uint8_t lowNibble = cpu->tmp & 0x0F;
            cpu->tmp <<= 4;
            cpu->tmp |= lowNibble;
            cpu->tmp == 0 ? (cpu->F.bits.Z = 0) : (cpu->F.bits.Z = 1);
            cpu->F.bits.N = 0;
            cpu->F.bits.H = 0;
            cpu->F.bits.C = 0;
            break;
        }
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

/****  CPU Control Commands ****/

void CCF(CPU *cpu)
{
    cpu->F.bits.N = 0;
    cpu->F.bits.H = 0;
    cpu->F.bits.C = ~cpu->F.bits.C;  // complement carry flag
}

void SCF(CPU *cpu)
{
    cpu->F.bits.N = 0;
    cpu->F.bits.H = 0;
    cpu->F.bits.C = 1;  
}

// NOP
void NOP(CPU *cpu)
{
}

// HALT
void HALT(CPU *cpu)
{

}

// STOP
void STOP(CPU *cpu)
{

}

// DI
void DI(CPU *cpu)
{
    cpu->IME = 0;  // set Interrupt Master Enable flag: enable interrupts
}

// EI
void EI(CPU *cpu)
{
    cpu->IME = 1;  // clear Interrupt Master Enable flag: disable interrupts
}

/****  Jump Commands ****/

// JP u16
void JP_u16(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->PC = (uint16_t)cpu->W | cpu->Z;
            break;
    }
}

// JP HL
void JP_HL(CPU *cpu)
{
    cpu->PC = (uint16_t)cpu->H | cpu->L;
}

// JP cc, u16
void JP_cc_u16(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);

            switch (cpu->instruction_register >> 3 & 0x03)   // check condition, end instruction if not met
            {
                case 0:
                    !cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 4) : (cpu->current_instruction->machine_cycles = 3);
                    break;
                case 1:
                    cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 4) : (cpu->current_instruction->machine_cycles = 3);
                    break;
                case 2:
                    !cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 4) : (cpu->current_instruction->machine_cycles = 3);
                    break;
                case 3:
                    cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 4) : (cpu->current_instruction->machine_cycles = 3);
                    break;
            }
            break;
        case 4:
            cpu->PC = (uint16_t)cpu->W | cpu->Z;
            break;
    }
}

// JR i8
void JR_i8(CPU *cpu)   
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);  // relative offset (-126, +129)
            break;
        case 3:
            if (cpu->tmp & 0x80)     // negative offset
                cpu->PC -= cpu->tmp;
            else                     // positive offset
                cpu->PC += cpu->tmp;
            break;
    }
}

// JR cc, i8
void JR_cc_i8(CPU *cpu)  
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->tmp = bus_read(cpu->address_bus);  // relative offset (-126, +129)

            switch (cpu->instruction_register >> 3 & 0x03)   // check condition, end instruction if not met
            {
                case 0:
                    !cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 3) : (cpu->current_instruction->machine_cycles = 2);
                    break;
                case 1:
                    cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 3) : (cpu->current_instruction->machine_cycles = 2);
                    break;
                case 2:
                    !cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 3) : (cpu->current_instruction->machine_cycles = 2);
                    break;
                case 3:
                    cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 3) : (cpu->current_instruction->machine_cycles = 2);
                    break;
            }

            break;
        case 3:
            cpu->PC += (int8_t)cpu->tmp;  // cast to int8_t - sign-extended when promoted to signed int
            break;
    }
}

// CALL u16
void CALL_u16(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->address_bus = cpu->SP;
            cpu->SP--;
            break;
        case 5:
            cpu->address_bus = cpu->SP;
            bus_write(cpu->address_bus, cpu->PC >> 8 & 0xFF);  // save PC high on stack
            cpu->PC = cpu->PC & 0x00FF | (uint16_t)cpu->W << 8;     // load PC high
            cpu->SP--;
            break;
        case 6:
            cpu->address_bus = cpu->SP;
            bus_write(cpu->address_bus, cpu->PC & 0xFF);   // save PC low on stack
            cpu->PC = cpu->PC & 0xFF00 | cpu->Z;                // load PC low
            break;
    }   
}

// CALL cc, u16
void CALL_cc_u16(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->PC;
            cpu->PC++;
            cpu->W = bus_read(cpu->address_bus);

            switch (cpu->instruction_register >> 3 & 0x03)  // check condition, end instruction if not met
            {
                case 0:
                    !cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 6) : (cpu->current_instruction->machine_cycles = 3);
                    break;
                case 1:
                    cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 6) : (cpu->current_instruction->machine_cycles = 3);
                    break;
                case 2:
                    !cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 6) : (cpu->current_instruction->machine_cycles = 3);
                    break;
                case 3:
                    cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 6) : (cpu->current_instruction->machine_cycles = 3);
                    break;
            }

            break;
        case 4:
            cpu->address_bus = cpu->SP;
            cpu->SP--;
            break;
        case 5:
            cpu->address_bus = cpu->SP;
            bus_write(cpu->address_bus, cpu->PC >> 8u & 0xFFu);  // save PC high on stack
            cpu->PC = cpu->PC & 0x00FF | (uint16_t)cpu->W << 8u;     // load PC high
            cpu->SP--;
            break;
        case 6:
            cpu->address_bus = cpu->SP;
            bus_write(cpu->address_bus, cpu->PC & 0xFFu);    // save PC low on stack
            cpu->PC = cpu->PC & 0xFF00u | cpu->Z;                 // load PC low
            break;
    } 
}

// RET
void RET(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 3:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->PC = (uint16_t)cpu->W << 8u | cpu->Z;
            break;
    }
}

// RET cc
void RET_cc(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            switch (cpu->instruction_register >> 3 & 0x03)  // check condition, end instruction if not met
            {
                case 0:
                    !cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 5) : (cpu->current_instruction->machine_cycles = 2);
                    break;
                case 1:
                    cpu->F.bits.Z ? (cpu->current_instruction->machine_cycles = 5) : (cpu->current_instruction->machine_cycles = 2);
                    break;
                case 2:
                    !cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 5) : (cpu->current_instruction->machine_cycles = 2);
                    break;
                case 3:
                    cpu->F.bits.C ? (cpu->current_instruction->machine_cycles = 5) : (cpu->current_instruction->machine_cycles = 2);
                    break;
            }
            break;
        case 3:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->Z = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 5:
            cpu->PC = (uint16_t)cpu->W << 8 | cpu->Z;
            break;
    }             
}

// RETI
void RETI(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->Z = bus_read(cpu->address_bus);
            cpu->IME = 1;  // re-enable interrupt
            break;
        case 3:
            cpu->address_bus = cpu->SP;
            cpu->SP++;
            cpu->W = bus_read(cpu->address_bus);
            break;
        case 4:
            cpu->PC = (uint16_t)cpu->W << 8u | cpu->Z;
            break;
    }
}

// RST n
void RST_n(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = cpu->SP;
            cpu->SP--;
            break;
        case 3:
            cpu->address_bus = cpu->SP;
            cpu->SP--;
            bus_write(cpu->address_bus, cpu->PC >> 8 & 0xFF);
            cpu->PC &= 0x00FF;
            break;
        case 4:
            cpu->address_bus = cpu->SP;
            bus_write(cpu->address_bus, cpu->PC & 0xFF);

            switch (cpu->instruction_register >> 3 & 0x07)
            {
                case 0:
                    cpu->PC = 0x0000;
                    break;
                case 1:
                    cpu->PC = 0x0008;
                    break;
                case 2:
                    cpu->PC = 0x0010;
                    break;
                case 3:
                    cpu->PC = 0x0018;
                    break;
                case 4:
                    cpu->PC = 0x0020;
                    break;
                case 5:
                    cpu->PC = 0x0028;
                    break;
                case 6:
                    cpu->PC = 0x0030;
                    break;
                case 7:
                    cpu->PC = 0x0038;
                    break;
            }

            break;
    }
}

/**** Single-bit Operation Commands ****/

// BIT n, r
void BIT_n_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->tmp = cpu->B;
                    break;
                case 1:
                    cpu->tmp = cpu->C;
                    break;
                case 2:
                    cpu->tmp = cpu->D;
                    break;
                case 3:
                    cpu->tmp = cpu->E;
                    break;
                case 4:
                    cpu->tmp = cpu->H;
                    break;
                case 5:
                    cpu->tmp = cpu->L;
                    break;
                case 7:
                    cpu->tmp = cpu->A;
                    break;
            }
            break;
        case 2:
        {
            uint8_t shift = cpu->instruction_register >> 3 & 0x07;
            cpu->F.bits.Z = !(cpu->tmp & 1 << shift);
            cpu->F.bits.H = 1;
            cpu->F.bits.N = 0;
            break;
        }
    }
}

// BIT n, (HL)
void BIT_n_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
        {
            uint8_t shift = cpu->instruction_register >> 3 & 0x07;
            cpu->F.bits.Z = !(cpu->tmp & 1 << shift);
            cpu->F.bits.H = 1;
            cpu->F.bits.N = 0;
            break;
        }
    }
}

// SET n, r
void SET_n_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
        {
            uint8_t shift = cpu->instruction_register >> 3 & 0x07;

            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->B |= 1 << shift;
                    break;
                case 1:
                    cpu->C |= 1 << shift;
                    break;
                case 2:
                    cpu->D |= 1 << shift;
                    break;
                case 3:
                    cpu->E |= 1 << shift;
                    break;
                case 4:
                    cpu->H |= 1 << shift;
                    break;
                case 5:
                    cpu->L |= 1 << shift;
                    break;
                case 7:
                    cpu->A |= 1 << shift;
                    break;
            }

            break;
        }
    }
}

// SET n, (HL)
void SET_n_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
        {
            uint8_t shift = cpu->instruction_register >> 3 & 0x07;
            cpu->tmp |= 1 << shift;
            break;
        }
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

// RES n, r
void RES_n_r(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
        {
            uint8_t shift = cpu->instruction_register >> 3 & 0x07;

            switch (cpu->instruction_register & 0x07)
            {
                case 0:
                    cpu->B &= ~(1U << shift);
                    break;
                case 1:
                    cpu->C &= ~(1U << shift);
                    break;
                case 2:
                    cpu->D &= ~(1U << shift);
                    break;
                case 3:
                    cpu->E &= ~(1U << shift);
                    break;
                case 4:
                    cpu->H &= ~(1U << shift);
                    break;
                case 5:
                    cpu->L &= ~(1U << shift);
                    break;
                case 7:
                    cpu->A &= ~(1U << shift);
                    break;
            }

            break;
        }
    }
}

// RES n, (HL)
void RES_n_ind_HL(CPU *cpu)
{
    switch (cpu->current_machine_cycle)
    {
        case 1:
            break;
        case 2:
            cpu->address_bus = (uint16_t)cpu->H << 8 | cpu->L;
            cpu->tmp = bus_read(cpu->address_bus);
            break;
        case 3:
        {
            uint8_t shift = cpu->instruction_register >> 3 & 0x07;
            cpu->tmp &= ~(1 << shift);
            break;
        }
        case 4:
            bus_write(cpu->address_bus, cpu->tmp);
            break;
    }
}

/**** instruction set table ****/
Instruction instruction_table[256] = /**** print string, opcode, bytes, machine cycles, instruction handler ****/
{
/********/  /* +0x0 */                                                   /* +0x1 */                                  /* +0x2 */                                                 /* +0x3 */                                     /* +0x4 */                                    /* +0x5 */                                  /* +0x6 */                                     /* +0x7 */                                 /* +0x8 */                                          /* +0x9 */                                /* +0xA */                                    /* +0xB */                              /* +0xC */                                   /* +0xD */                             /* +0xE */                                    /* +0xF */ 
/* 0x0+ */  { "NOP",                 0x00, 1, 1, &NOP                   }, { "LD BC, u16", 0x01, 3, 3, &LD_rr_u16   }, { "LD (BC), A",         0x02, 1, 2, &LD_ind_BC_A          }, { "INC BC",     0x03, 1, 2, &INC_rr         }, { "INC B",        0x04, 1, 1, &INC_r       }, { "DEC B",      0x05, 1, 1, &DEC_r       }, { "LD B, u8",    0x06, 2, 2, &LD_r_u8      }, { "RLCA",       0x07, 1, 1, &RLCA        }, { "LD (u16), SP",  0x08, 3, 5, &LD_ind_u16_SP    }, { "ADD HL, BC", 0x09, 1, 2, &ADD_HL_rr }, { "LD A, (BC)",  0x0A, 1, 2, &LD_A_ind_BC  }, { "DEC BC",    0x0B, 1, 2, &DEC_rr   }, { "INC C",       0x0C, 1, 1, &INC_r       }, { "DEC C",    0x0D, 1, 1, &DEC_r    }, { "LD C, u8",    0x0E, 2, 2, &LD_r_u8      }, { "RRCA",     0x0F, 1, 1, &RRCA     },
/* 0x1+ */  { "STOP",                0x10, 2, 1, &STOP                  }, { "LD DE, u16", 0x11, 3, 3, &LD_rr_u16   }, { "LD (DE), A",         0x12, 1, 2, &LD_ind_DE_A          }, { "INC DE",     0x13, 1, 2, &INC_rr         }, { "INC D",        0x14, 1, 1, &INC_r       }, { "DEC D",      0x15, 1, 1, &DEC_r       }, { "LD D, u8",    0x16, 2, 2, &LD_r_u8      }, { "RLA",        0x17, 1, 1, &RLA         }, { "JR, i8",        0x18, 2, 3, &JR_i8            }, { "ADD HL, DE", 0x19, 1, 2, &ADD_HL_rr }, { "LD A, (DE)",  0x1A, 1, 2, &LD_A_ind_DE  }, { "DEC DE",    0x1B, 1, 2, &DEC_rr   }, { "INC E",       0x1C, 1, 1, &INC_r       }, { "DEC E",    0x1D, 1, 1, &DEC_r    }, { "LD E, u8",    0x1E, 2, 2, &LD_r_u8      }, { "RRA",      0x1F, 1, 1, &RRA      },
/* 0x2+ */  { "JR NZ, i8",           0x20, 2, 3, &JR_cc_i8              }, { "LD HL, u16", 0x21, 3, 3, &LD_rr_u16   }, { "LDI (HL), A",        0x22, 1, 2, &LDI_ind_HL_A         }, { "INC HL",     0x23, 1, 2, &INC_rr         }, { "INC H",        0x24, 1, 1, &INC_r       }, { "DEC H",      0x25, 1, 1, &DEC_r       }, { "LD H, u8",    0x26, 2, 2, &LD_r_u8      }, { "DAA",        0x27, 1, 1, &DAA         }, { "JR Z, i8",      0x28, 2, 3, &JR_cc_i8         }, { "ADD HL, HL", 0x29, 1, 2, &ADD_HL_rr }, { "LDI A, (HL)", 0x2A, 1, 2, &LDI_A_ind_HL }, { "DEC HL",    0x2B, 1, 2, &DEC_rr   }, { "INC L",       0x2C, 1, 1, &INC_r       }, { "DEC L",    0x2D, 1, 1, &DEC_r    }, { "LD L, u8",    0x2E, 2, 2, &LD_r_u8      }, { "CPL",      0x2F, 1, 1, &CPL      },                       
/* 0x3+ */  { "JR C, i8",            0x30, 2, 3, &JR_cc_i8              }, { "LD SP, u16", 0x31, 3, 3, &LD_rr_u16   }, { "LDD (HL), A",        0x32, 1, 2, &LDD_ind_HL_A         }, { "INC SP",     0x33, 1, 2, &INC_rr         }, { "INC (HL)",     0x34, 1, 3, &INC_ind_HL  }, { "DEC (HL)",   0x35, 1, 3, &DEC_ind_HL  }, { "LD (HL), u8", 0x36, 2, 3, &LD_ind_HL_u8 }, { "SCF",        0x37, 1, 1, &SCF         }, { "JR C, i8",      0x38, 2, 3, &JR_cc_i8         }, { "ADD HL, SP", 0x39, 1, 2, &ADD_HL_rr }, { "LDD A, (HL)", 0x3A, 1, 2, &LDD_A_ind_HL }, { "DEC SP",    0x3B, 1, 2, &DEC_rr   }, { "INC A",       0x3C, 1, 1, &INC_r       }, { "DEC A",    0x3D, 1, 1, &DEC_r    }, { "LD A, u8",    0x3E, 2, 2, &LD_r_u8      }, { "CCF",      0x3F, 1, 1, &CPL      }, 
/* 0x4+ */  { "LD B, B",             0x40, 1, 1, &LD_r1_r2              }, { "LD B, C",    0x41, 1, 1, &LD_r1_r2    }, { "LD B, D",            0x42, 1, 1, &LD_r1_r2             }, { "LD B, E",    0x43, 1, 1, &LD_r1_r2       }, { "LD B, H",      0x44, 1, 1, &LD_r1_r2    }, { "LD B, L",    0x45, 1, 1, &LD_r1_r2    }, { "LD B, (HL)",  0x46, 1, 2, &LD_r_ind_HL  }, { "LD B, A",    0x47, 1, 1, &LD_r1_r2    }, { "LD C, B",       0x48, 1, 1, &LD_r1_r2         }, { "LD C, C",    0x49, 1, 1, &LD_r1_r2  }, { "LD C, D",     0x4A, 1, 1, &LD_r1_r2     }, { "LD C, E",   0x4B, 1, 1, &LD_r1_r2 }, { "LD C, H",     0x4C, 1, 1, &LD_r1_r2    }, { "LD C, L",  0x4D, 1, 1, &LD_r1_r2 }, { "LD C, (HL)",  0x4E, 1, 2, &LD_r_ind_HL  }, { "LD C, A",  0x4F, 1, 1, &LD_r1_r2 },              
/* 0x5+ */  { "LD D, B",             0x50, 1, 1, &LD_r1_r2              }, { "LD D, C",    0x51, 1, 1, &LD_r1_r2    }, { "LD D, D",            0x52, 1, 1, &LD_r1_r2             }, { "LD D, E",    0x53, 1, 1, &LD_r1_r2       }, { "LD D, H",      0x54, 1, 1, &LD_r1_r2    }, { "LD D, L",    0x55, 1, 1, &LD_r1_r2    }, { "LD D, (HL)",  0x56, 1, 2, &LD_r_ind_HL  }, { "LD D, A",    0x57, 1, 1, &LD_r1_r2    }, { "LD E, B",       0x58, 1, 1, &LD_r1_r2         }, { "LD E, C",    0x59, 1, 1, &LD_r1_r2  }, { "LD E, D",     0x5A, 1, 1, &LD_r1_r2     }, { "LD E, E",   0x5B, 1, 1, &LD_r1_r2 }, { "LD E, H",     0x5C, 1, 1, &LD_r1_r2    }, { "LD E, L",  0x5D, 1, 1, &LD_r1_r2 }, { "LD E, (HL)",  0x5E, 1, 2, &LD_r_ind_HL  }, { "LD E, A",  0x5F, 1, 1, &LD_r1_r2 },
/* 0x6+ */  { "LD H, B",             0x60, 1, 1, &LD_r1_r2              }, { "LD H, C",    0x61, 1, 1, &LD_r1_r2    }, { "LD H, D",            0x62, 1, 1, &LD_r1_r2             }, { "LD H, E",    0x63, 1, 1, &LD_r1_r2       }, { "LD H, H",      0x64, 1, 1, &LD_r1_r2    }, { "LD H, L",    0x65, 1, 1, &LD_r1_r2    }, { "LD H, (HL)",  0x66, 1, 2, &LD_r_ind_HL  }, { "LD H, A",    0x67, 1, 1, &LD_r1_r2    }, { "LD L, B",       0x68, 1, 1, &LD_r1_r2         }, { "LD L, C",    0x69, 1, 1, &LD_r1_r2  }, { "LD L, D",     0x6A, 1, 1, &LD_r1_r2     }, { "LD L, E",   0x6B, 1, 1, &LD_r1_r2 }, { "LD L, H",     0x6C, 1, 1, &LD_r1_r2    }, { "LD L, L",  0x6D, 1, 1, &LD_r1_r2 }, { "LD L, (HL)",  0x6E, 1, 2, &LD_r_ind_HL  }, { "LD L, A",  0x6F, 1, 1, &LD_r1_r2 },
/* 0x7+ */  { "LD (HL), B",          0x70, 1, 2, &LD_ind_HL_r           }, { "LD (HL), C", 0x71, 1, 2, &LD_ind_HL_r }, { "LD (HL), D",         0x72, 1, 2, &LD_ind_HL_r          }, { "LD (HL), E", 0x73, 1, 2, &LD_ind_HL_r    }, { "LD (HL), H",   0x74, 1, 2, &LD_ind_HL_r }, { "LD (HL), L", 0x75, 1, 2, &LD_ind_HL_r }, { "HALT",        0x76, 1, 1, &HALT         }, { "LD (HL), A", 0x77, 1, 2, &LD_ind_HL_r }, { "LD A, B",       0x78, 1, 1, &LD_r1_r2         }, { "LD A, C",    0x79, 1, 1, &LD_r1_r2  }, { "LD A, D",     0x7A, 1, 1, &LD_r1_r2     }, { "LD A, E",   0x7B, 1, 1, &LD_r1_r2 }, { "LD A, H",     0x7C, 1, 1, &LD_r1_r2    }, { "LD A, L",  0x7D, 1, 1, &LD_r1_r2 }, { "LD A, (HL)",  0x7E, 1, 2, &LD_r_ind_HL  }, { "LD A, A",  0x7F, 1, 1, &LD_r1_r2 },
/* 0x8+ */  { "ADD A, B",            0x80, 1, 1, &ADD_A_r               }, { "ADD A, C",   0x81, 1, 1, &ADD_A_r     }, { "ADD A, D",           0x82, 1, 1, &ADD_A_r              }, { "ADD A, E",   0x83, 1, 1, &ADD_A_r        }, { "ADD A, H",     0x84, 1, 1, &ADD_A_r     }, { "ADD A, L",   0x85, 1, 1, &ADD_A_r     }, { "ADD A, (HL)", 0x86, 1, 2, &ADD_A_ind_HL }, { "ADD A, A",   0x87, 1, 1, &ADD_A_r     }, { "ADC A, B",      0x88, 1, 1, &ADC_A_r          }, { "ADC A, C",   0x89, 1, 1, &ADC_A_r   }, { "ADC A, D",    0x8A, 1, 1, &ADC_A_r      }, { "ADC A, E",  0x8B, 1, 1, &ADC_A_r  }, { "ADC A, H",    0x8C, 1, 1, &ADC_A_r     }, { "ADC A, L", 0x8D, 1, 1, &ADC_A_r  }, { "ADC A, (HL)", 0x8E, 1, 2, &ADC_A_ind_HL }, { "ADC A, A", 0x8F, 1, 1, &ADC_A_r  },                                          
/* 0x9+ */  { "SUB A, B",            0x90, 1, 1, &SUB_A_r               }, { "SUB A, C",   0x91, 1, 1, &SUB_A_r     }, { "SUB A, D",           0x92, 1, 1, &SUB_A_r              }, { "SUB A, E",   0x93, 1, 1, &SUB_A_r        }, { "SUB A, H",     0x94, 1, 1, &SUB_A_r     }, { "SUB A, L",   0x95, 1, 1, &SUB_A_r     }, { "SUB A, (HL)", 0x96, 1, 2, &SUB_A_ind_HL }, { "SUB A, A",   0x97, 1, 1, &SUB_A_r     }, { "SBC A, B",      0x98, 1, 1, &SBC_A_r          }, { "SBC A, C",   0x99, 1, 1, &SBC_A_r   }, { "SBC A, D",    0x9A, 1, 1, &SBC_A_r      }, { "SBC A, E",  0x9B, 1, 1, &SBC_A_r  }, { "SBC A, H",    0x9C, 1, 1, &SBC_A_r     }, { "SBC A, L", 0x9D, 1, 1, &SBC_A_r  }, { "SBC A, (HL)", 0x9E, 1, 2, &SBC_A_ind_HL }, { "SBC A, A", 0x9F, 1, 1, &SBC_A_r  },            
/* 0xA+ */  { "AND A, B",            0xA0, 1, 1, &AND_A_r               }, { "AND A, C",   0xA1, 1, 1, &AND_A_r     }, { "AND A, D",           0xA2, 1, 1, &AND_A_r              }, { "AND A, E",   0xA3, 1, 1, &AND_A_r        }, { "AND A, H",     0xA4, 1, 1, &AND_A_r     }, { "AND A, L",   0xA5, 1, 1, &AND_A_r     }, { "AND A, (HL)", 0xA6, 1, 2, &AND_A_ind_HL }, { "AND A, A",   0xA7, 1, 1, &AND_A_r     }, { "XOR A, B",      0xA8, 1, 1, &XOR_A_r          }, { "XOR A, C",   0xA9, 1, 1, &XOR_A_r   }, { "XOR A, D",    0xAA, 1, 1, &XOR_A_r      }, { "XOR A, E",  0xAB, 1, 1, &XOR_A_r  }, { "XOR A, H",    0xAC, 1, 1, &XOR_A_r     }, { "XOR A, L", 0xAD, 1, 1, &XOR_A_r  }, { "XOR A, (HL)", 0xAE, 1, 2, &XOR_A_ind_HL }, { "XOR A, A", 0xAF, 1, 1, &XOR_A_r  },                                                                                                                                               
/* 0xB+ */  { "OR A, B",             0xB0, 1, 1, &OR_A_r                }, { "OR A, C",    0xB1, 1, 1, &OR_A_r      }, { "OR A, D",            0xB2, 1, 1, &OR_A_r               }, { "OR A, E",    0xB3, 1, 1, &OR_A_r         }, { "OR A, H",      0xB4, 1, 1, &OR_A_r      }, { "OR A, L",    0xB5, 1, 1, &OR_A_r      }, { "OR A, (HL)",  0xB6, 1, 2, &OR_A_ind_HL  }, { "OR A, A",    0xB7, 1, 1, &OR_A_r      }, { "CP A, B",       0xB8, 1, 1, &CP_A_r           }, { "CP A, C",    0xB9, 1, 1, &CP_A_r    }, { "CP A, D",     0xBA, 1, 1, &CP_A_r       }, { "CP A, E",   0xBB, 1, 1, &CP_A_r   }, { "CP A, H",     0xBC, 1, 1, &CP_A_r      }, { "CP A, L",  0xBD, 1, 1, &CP_A_r   }, { "CP A, (HL)",  0xBE, 1, 2, &CP_A_ind_HL  }, { "CP A, A",  0xBF, 1, 1, &CP_A_r   },                                                                                                                                                                                                                                           
/* 0xC+ */  { "RET NZ",              0xC0, 1, 5, &RET_cc                }, { "POP BC",     0xC1, 1, 3, &POP_rr      }, { "JP NZ, u16",         0xC2, 3, 4, &JP_cc_u16            }, { "JP, u16",    0xC3, 3, 4, &JP_u16         }, { "CALL NZ, u16", 0xC4, 3, 6, &CALL_cc_u16 }, { "PUSH BC",    0xC5, 1, 4, &PUSH_rr     }, { "ADD A, u8",   0xC6, 2, 2, &ADD_A_u8     }, { "RST $00",    0xC7, 1, 4, &RST_n       }, { "RET Z",         0xC8, 1, 5, &RET_cc           }, { "RET",        0xC9, 1, 4, &RET       }, { "JP Z, u16",   0xCA, 3, 4, &JP_cc_u16    }, { "PREFIX CB", 0xCB, 1, 1, NULL      }, { "CALL Z, u16", 0xCC, 3, 6, &CALL_cc_u16 }, { "CALL u16", 0xCD, 3, 6, &CALL_u16 }, { "ADC A, u8",   0xCE, 2, 2, &ADC_A_u8     }, { "RST $08",  0xCF, 1, 4, &RST_n    },
/* 0xD+ */  { "RET NC",              0xD0, 1, 5, &RET_cc                }, { "POP DE",     0xD1, 1, 3, &POP_rr      }, { "JP NC, u16",         0xD2, 3, 4, &JP_cc_u16            }, { "INVALID",    0xD3, 0, 0, NULL            }, { "CALL NC, u16", 0xD4, 3, 6, &CALL_cc_u16 }, { "PUSH DE",    0xD5, 1, 4, &PUSH_rr     }, { "SUB A, u8",   0xD6, 2, 2, &SUB_A_u8     }, { "RST $10",    0xD7, 1, 4, &RST_n       }, { "RET C",         0xD8, 1, 5, &RET_cc           }, { "RETI",       0xD9, 1, 4, &RETI      }, { "JP C, u16",   0xDA, 3, 4, &JP_cc_u16    }, { "INVALID",   0xDB, 0, 0, NULL      }, { "CALL C, u16", 0xDC, 3, 6, &CALL_cc_u16 }, { "INVALID",  0xDD, 0, 0, NULL      }, { "SBC A, u8",   0xDE, 2, 2, &SBC_A_u8     }, { "RST 18",   0xDF, 1, 4, &RST_n    },
/* 0xE+ */  { "LD (0xFF00 + u8), A", 0xE0, 2, 3, &LD_ind_FF00_plus_u8_A }, { "POP HL",     0xE1, 1, 3, &POP_rr      }, { "LD (0xFF00 + C), A", 0xE2, 1, 2, &LD_ind_FF00_plus_C_A }, { "INVALID",    0xE3, 0, 0, NULL            }, { "INVALID",      0xE4, 0, 0, NULL         }, { "PUSH HL",    0xE5, 1, 4, &PUSH_rr     }, { "AND A, u8",   0xE6, 2, 2, &AND_A_u8     }, { "RST $20",    0xE7, 1, 4, &RST_n       }, { "ADD SP, i8",    0xE8, 2, 4, &ADD_SP_i8        }, { "JP, HL",     0xE9, 1, 1, &JP_HL     }, { "LD (u16), A", 0xEA, 3, 4, &LD_ind_u16_A }, { "INVALID",   0xEB, 0, 0, NULL      }, { "INVALID",     0xEC, 0, 0, NULL         }, { "INVALID",  0xED, 0, 0, NULL      }, { "XOR A, u8",   0xEE, 2, 2, &XOR_A_u8     }, { "RST $28",  0xEF, 1, 4, &RST_n    },
/* 0xF+ */  { "LD A, (0xFF00 + u8)", 0xF0, 2, 3, &LD_A_ind_FF00_plus_u8 }, { "POP AF",     0xF1, 1, 3, &POP_rr      }, { "LD A, (0xFF00 + C)", 0xF2, 1, 2, &LD_A_ind_FF00_plus_C }, { "DI",         0xF3, 1, 1, &DI             }, { "INVALID",      0xF4, 0, 0, NULL         }, { "PUSH AF",    0xF5, 1, 4, &PUSH_rr     }, { "OR A, u8",    0xF6, 2, 2, &OR_A_u8      }, { "RST $30",    0xF7, 1, 4, &RST_n       }, { "LD HL SP + i8", 0xF8, 2, 3, &LD_HL_SP_plus_i8 }, { "LD SP, HL",  0xF9, 1, 2, &LD_SP_HL  }, { "LD A, (u16)", 0xFA, 3, 4, &LD_A_ind_u16 }, { "EI",        0xFB, 1, 1, &EI       }, { "INVALID",     0xFC, 0, 0, NULL         }, { "INVALID",  0xFD, 0, 0, NULL      }, { "CP A, u8",    0xFE, 2, 2, &CP_A_u8      }, { "RST $38",  0xFF, 1, 4, &RST_n    },
};

/**** extended instruction set table (CB-prefixed) ****/
Instruction extended_instruction_table[256] = /**** print string, opcode, bytes, machine cycles, instruction handler ****/
{
/********/   /* +0x0 */                            /* +0x1 */                           /* +0x2 */                           /* +0x3 */                           /* +0x4 */                           /* +0x5 */                           /* +0x6 */                                   /* +0x7 */                           /* +0x8 */                           /* +0x9 */                           /* +0xA */                           /* +0xB */                           /* +0xC */                           /* +0xD */                           /* +0xE */                                   /* +0xF */ 
/* 0x0+ */  { "RLC B",    0x00, 2, 2, &RLC_r  }, { "RLC C",    0x01, 2, 2, &RLC_r  }, { "RLC D",    0x02, 2, 2, &RLC_r  }, { "RLC E",    0x03, 2, 2, &RLC_r  }, { "RLC H",    0x04, 2, 2, &RLC_r  }, { "RLC L",    0x05, 2, 2, &RLC_r  }, { "RLC (HL)",    0x06, 2, 4, &RLC_ind_HL  }, { "RLC A",    0x07, 2, 2, &RLC_r  }, { "RRC B",    0x08, 2, 2, &RRC_r  }, { "RRC C",    0x09, 2, 2, &RRC_r  }, { "RRC D",    0x0A, 2, 2, &RRC_r  }, { "RRC E",    0x0B, 2, 2, &RRC_r  }, { "RRC H",    0x0C, 2, 2, &RRC_r  }, { "RRC L",    0x0D, 2, 2, &RRC_r  }, { "RRC (HL)",    0x0E, 2, 4, &RRC_ind_HL  }, { "RRC A",    0x0F, 2, 2, &RRC_r  }, 
/* 0x1+ */  { "RL B",     0x10, 2, 2, &RL_r   }, { "RL C",     0x11, 2, 2, &RL_r   }, { "RL D",     0x12, 2, 2, &RL_r   }, { "RL E",     0x13, 2, 2, &RL_r   }, { "RL H",     0x14, 2, 2, &RL_r   }, { "RL L",     0x15, 2, 2, &RL_r   }, { "RL (HL)",     0x16, 2, 4, &RL_ind_HL   }, { "RL A",     0x17, 2, 2, &RL_r   }, { "RR B",     0x18, 2, 2, &RR_r   }, { "RR C",     0x19, 2, 2, &RR_r   }, { "RR D",     0x1A, 2, 2, &RR_r   }, { "RR E",     0x1B, 2, 2, &RR_r   }, { "RR H",     0x1C, 2, 2, &RR_r   }, { "RR L",     0x1D, 2, 2, &RR_r   }, { "RR (HL)",     0x1E, 2, 4, &RR_ind_HL   }, { "RR A",     0x1F, 2, 2, &RR_r   },          
/* 0x2+ */  { "SLA B",    0x20, 2, 2, &SLA_r  }, { "SLA C",    0x21, 2, 2, &SLA_r  }, { "SLA D",    0x22, 2, 2, &SLA_r  }, { "SLA E",    0x23, 2, 2, &SLA_r  }, { "SLA H",    0x24, 2, 2, &SLA_r  }, { "SLA L",    0x25, 2, 2, &SLA_r  }, { "SLA (HL)",    0x26, 2, 4, &SLA_ind_HL  }, { "SLA A",    0x27, 2, 2, &SLA_r  }, { "SRA B",    0x28, 2, 2, &SRA_r  }, { "SRA C",    0x29, 2, 2, &SRA_r  }, { "SRA D",    0x2A, 2, 2, &SRA_r  }, { "SRA E",    0x2B, 2, 2, &SRA_r  }, { "SRA H",    0x2C, 2, 2, &SRA_r  }, { "SRA L",    0x2D, 2, 2, &SRA_r  }, { "SRA (HL)",    0x2E, 2, 4, &SRA_ind_HL  }, { "SRA A",    0x2F, 2, 2, &SRA_r  },      
/* 0x3+ */  { "SWAP B",   0x30, 2, 2, &SWAP_r }, { "SWAP C",   0x31, 2, 2, &SWAP_r }, { "SWAP D",   0x32, 2, 2, &SWAP_r }, { "SWAP E",   0x33, 2, 2, &SWAP_r }, { "SWAP H",   0x34, 2, 2, &SWAP_r }, { "SWAP L",   0x35, 2, 2, &SWAP_r }, { "SWAP (HL)",   0x36, 2, 4, &SWAP_ind_HL }, { "SWAP A",   0x37, 2, 2, &SWAP_r }, { "SRL B",    0x28, 2, 2, &SRL_r  }, { "SRL C",    0x29, 2, 2, &SRL_r  }, { "SRL D",    0x2A, 2, 2, &SRL_r  }, { "SRL E",    0x2B, 2, 2, &SRL_r  }, { "SRL H",    0x2C, 2, 2, &SRL_r  }, { "SRL L",    0x2D, 2, 2, &SRL_r  }, { "SRL (HL)",    0x2E, 2, 4, &SRL_ind_HL  }, { "SRL A",    0x2F, 2, 2, &SRL_r  }, 
/* 0x4+ */  { "BIT 0, B", 0x40, 2, 2, &BIT_n_r}, { "BIT 0, C", 0x41, 2, 2, &BIT_n_r}, { "BIT 0, D", 0x42, 2, 2, &BIT_n_r}, { "BIT 0, E", 0x43, 2, 2, &BIT_n_r}, { "BIT 0, H", 0x44, 2, 2, &BIT_n_r}, { "BIT 0, L", 0x45, 2, 2, &BIT_n_r}, { "BIT 0, (HL)", 0x46, 2, 3, &BIT_n_ind_HL}, { "BIT 0, A", 0x47, 2, 2, &BIT_n_r}, { "BIT 1, B", 0x48, 2, 2, &BIT_n_r}, { "BIT 1, C", 0x49, 2, 2, &BIT_n_r}, { "BIT 1, D", 0x4A, 2, 2, &BIT_n_r}, { "BIT 1, E", 0x4B, 2, 2, &BIT_n_r}, { "BIT 1, H", 0x4C, 2, 2, &BIT_n_r}, { "BIT 1, L", 0x4D, 2, 2, &BIT_n_r}, { "BIT 1, (HL)", 0x4E, 2, 3, &BIT_n_ind_HL}, { "BIT 1, A", 0x4F, 2, 2, &BIT_n_r}, 
/* 0x5+ */  { "BIT 2, B", 0x50, 2, 2, &BIT_n_r}, { "BIT 2, C", 0x51, 2, 2, &BIT_n_r}, { "BIT 2, D", 0x52, 2, 2, &BIT_n_r}, { "BIT 2, E", 0x53, 2, 2, &BIT_n_r}, { "BIT 2, H", 0x54, 2, 2, &BIT_n_r}, { "BIT 2, L", 0x55, 2, 2, &BIT_n_r}, { "BIT 2, (HL)", 0x56, 2, 3, &BIT_n_ind_HL}, { "BIT 2, A", 0x57, 2, 2, &BIT_n_r}, { "BIT 3, B", 0x58, 2, 2, &BIT_n_r}, { "BIT 3, C", 0x59, 2, 2, &BIT_n_r}, { "BIT 3, D", 0x5A, 2, 2, &BIT_n_r}, { "BIT 3, E", 0x5B, 2, 2, &BIT_n_r}, { "BIT 3, H", 0x5C, 2, 2, &BIT_n_r}, { "BIT 3, L", 0x5D, 2, 2, &BIT_n_r}, { "BIT 3, (HL)", 0x5E, 2, 3, &BIT_n_ind_HL}, { "BIT 3, A", 0x5F, 2, 2, &BIT_n_r}, 
/* 0x6+ */  { "BIT 4, B", 0x60, 2, 2, &BIT_n_r}, { "BIT 4, C", 0x61, 2, 2, &BIT_n_r}, { "BIT 4, D", 0x62, 2, 2, &BIT_n_r}, { "BIT 4, E", 0x63, 2, 2, &BIT_n_r}, { "BIT 4, H", 0x64, 2, 2, &BIT_n_r}, { "BIT 4, L", 0x65, 2, 2, &BIT_n_r}, { "BIT 4, (HL)", 0x66, 2, 3, &BIT_n_ind_HL}, { "BIT 4, A", 0x67, 2, 2, &BIT_n_r}, { "BIT 5, B", 0x68, 2, 2, &BIT_n_r}, { "BIT 5, C", 0x69, 2, 2, &BIT_n_r}, { "BIT 5, D", 0x6A, 2, 2, &BIT_n_r}, { "BIT 5, E", 0x6B, 2, 2, &BIT_n_r}, { "BIT 5, H", 0x6C, 2, 2, &BIT_n_r}, { "BIT 5, L", 0x6D, 2, 2, &BIT_n_r}, { "BIT 5, (HL)", 0x6E, 2, 3, &BIT_n_ind_HL}, { "BIT 5, A", 0x6F, 2, 2, &BIT_n_r}, 
/* 0x7+ */  { "BIT 6, B", 0x70, 2, 2, &BIT_n_r}, { "BIT 0, 6", 0x71, 2, 2, &BIT_n_r}, { "BIT 6, D", 0x72, 2, 2, &BIT_n_r}, { "BIT 6, E", 0x73, 2, 2, &BIT_n_r}, { "BIT 6, H", 0x74, 2, 2, &BIT_n_r}, { "BIT 6, L", 0x75, 2, 2, &BIT_n_r}, { "BIT 6, (HL)", 0x76, 2, 3, &BIT_n_ind_HL}, { "BIT 6, A", 0x77, 2, 2, &BIT_n_r}, { "BIT 7, B", 0x78, 2, 2, &BIT_n_r}, { "BIT 7, C", 0x79, 2, 2, &BIT_n_r}, { "BIT 7, D", 0x7A, 2, 2, &BIT_n_r}, { "BIT 7, E", 0x7B, 2, 2, &BIT_n_r}, { "BIT 7, H", 0x7C, 2, 2, &BIT_n_r}, { "BIT 7, L", 0x7D, 2, 2, &BIT_n_r}, { "BIT 7, (HL)", 0x7E, 2, 3, &BIT_n_ind_HL}, { "BIT 7, A", 0x7F, 2, 2, &BIT_n_r}, 
/* 0x8+ */  { "RES 0, B", 0x80, 2, 2, &RES_n_r}, { "RES 0, C", 0x81, 2, 2, &RES_n_r}, { "RES 0, D", 0x82, 2, 2, &RES_n_r}, { "RES 0, E", 0x83, 2, 2, &RES_n_r}, { "RES 0, H", 0x84, 2, 2, &RES_n_r}, { "RES 0, L", 0x85, 2, 2, &RES_n_r}, { "RES 0, (HL)", 0x86, 2, 4, &RES_n_ind_HL}, { "RES 0, A", 0x87, 2, 2, &RES_n_r}, { "RES 1, B", 0x88, 2, 2, &RES_n_r}, { "RES 1, C", 0x89, 2, 2, &RES_n_r}, { "RES 1, D", 0x8A, 2, 2, &RES_n_r}, { "RES 1, E", 0x8B, 2, 2, &RES_n_r}, { "RES 1, H", 0x8C, 2, 2, &RES_n_r}, { "RES 1, L", 0x8D, 2, 2, &RES_n_r}, { "RES 1, (HL)", 0x8E, 2, 4, &RES_n_ind_HL}, { "RES 1, A", 0x8F, 2, 2, &RES_n_r}, 
/* 0x9+ */  { "RES 2, B", 0x90, 2, 2, &RES_n_r}, { "RES 2, C", 0x91, 2, 2, &RES_n_r}, { "RES 2, D", 0x92, 2, 2, &RES_n_r}, { "RES 2, E", 0x93, 2, 2, &RES_n_r}, { "RES 2, H", 0x94, 2, 2, &RES_n_r}, { "RES 2, L", 0x95, 2, 2, &RES_n_r}, { "RES 2, (HL)", 0x96, 2, 4, &RES_n_ind_HL}, { "RES 2, A", 0x97, 2, 2, &RES_n_r}, { "RES 3, B", 0x98, 2, 2, &RES_n_r}, { "RES 3, C", 0x99, 2, 2, &RES_n_r}, { "RES 3, D", 0x9A, 2, 2, &RES_n_r}, { "RES 3, E", 0x9B, 2, 2, &RES_n_r}, { "RES 3, H", 0x9C, 2, 2, &RES_n_r}, { "RES 3, L", 0x9D, 2, 2, &RES_n_r}, { "RES 3, (HL)", 0x9E, 2, 4, &RES_n_ind_HL}, { "RES 3, A", 0x9F, 2, 2, &RES_n_r}, 
/* 0xA+ */  { "RES 4, B", 0xA0, 2, 2, &RES_n_r}, { "RES 4, C", 0xA1, 2, 2, &RES_n_r}, { "RES 4, D", 0xA2, 2, 2, &RES_n_r}, { "RES 4, E", 0xA3, 2, 2, &RES_n_r}, { "RES 4, H", 0xA4, 2, 2, &RES_n_r}, { "RES 4, L", 0xA5, 2, 2, &RES_n_r}, { "RES 4, (HL)", 0xA6, 2, 4, &RES_n_ind_HL}, { "RES 4, A", 0xA7, 2, 2, &RES_n_r}, { "RES 5, B", 0xA8, 2, 2, &RES_n_r}, { "RES 5, C", 0xA9, 2, 2, &RES_n_r}, { "RES 5, D", 0xAA, 2, 2, &RES_n_r}, { "RES 5, E", 0xAB, 2, 2, &RES_n_r}, { "RES 5, H", 0xAC, 2, 2, &RES_n_r}, { "RES 5, L", 0xAD, 2, 2, &RES_n_r}, { "RES 5, (HL)", 0xAE, 2, 4, &RES_n_ind_HL}, { "RES 5, A", 0xAF, 2, 2, &RES_n_r}, 
/* 0xB+ */  { "RES 6, B", 0xB0, 2, 2, &RES_n_r}, { "RES 0, 6", 0xB1, 2, 2, &RES_n_r}, { "RES 6, D", 0xB2, 2, 2, &RES_n_r}, { "RES 6, E", 0xB3, 2, 2, &RES_n_r}, { "RES 6, H", 0xB4, 2, 2, &RES_n_r}, { "RES 6, L", 0xB5, 2, 2, &RES_n_r}, { "RES 6, (HL)", 0xB6, 2, 4, &RES_n_ind_HL}, { "RES 6, A", 0xB7, 2, 2, &RES_n_r}, { "RES 7, B", 0xB8, 2, 2, &RES_n_r}, { "RES 7, C", 0xB9, 2, 2, &RES_n_r}, { "RES 7, D", 0xBA, 2, 2, &RES_n_r}, { "RES 7, E", 0xBB, 2, 2, &RES_n_r}, { "RES 7, H", 0xBC, 2, 2, &RES_n_r}, { "RES 7, L", 0xBD, 2, 2, &RES_n_r}, { "RES 7, (HL)", 0xBE, 2, 4, &RES_n_ind_HL}, { "RES 7, A", 0xBF, 2, 2, &RES_n_r}, 
/* 0xC+ */  { "SET 0, B", 0xC0, 2, 2, &SET_n_r}, { "SET 0, C", 0xC1, 2, 2, &SET_n_r}, { "SET 0, D", 0xC2, 2, 2, &SET_n_r}, { "SET 0, E", 0xC3, 2, 2, &SET_n_r}, { "SET 0, H", 0xC4, 2, 2, &SET_n_r}, { "SET 0, L", 0xC5, 2, 2, &SET_n_r}, { "SET 0, (HL)", 0xC6, 2, 4, &SET_n_ind_HL}, { "SET 0, A", 0xC7, 2, 2, &SET_n_r}, { "SET 1, B", 0xC8, 2, 2, &SET_n_r}, { "SET 1, C", 0xC9, 2, 2, &SET_n_r}, { "SET 1, D", 0xCA, 2, 2, &SET_n_r}, { "SET 1, E", 0xCB, 2, 2, &SET_n_r}, { "SET 1, H", 0xCC, 2, 2, &SET_n_r}, { "SET 1, L", 0xCD, 2, 2, &SET_n_r}, { "SET 1, (HL)", 0xCE, 2, 4, &SET_n_ind_HL}, { "SET 1, A", 0xCF, 2, 2, &SET_n_r}, 
/* 0xD+ */  { "SET 2, B", 0xD0, 2, 2, &SET_n_r}, { "SET 2, C", 0xD1, 2, 2, &SET_n_r}, { "SET 2, D", 0xD2, 2, 2, &SET_n_r}, { "SET 2, E", 0xD3, 2, 2, &SET_n_r}, { "SET 2, H", 0xD4, 2, 2, &SET_n_r}, { "SET 2, L", 0xD5, 2, 2, &SET_n_r}, { "SET 2, (HL)", 0xD6, 2, 4, &SET_n_ind_HL}, { "SET 2, A", 0xD7, 2, 2, &SET_n_r}, { "SET 3, B", 0xD8, 2, 2, &SET_n_r}, { "SET 3, C", 0xD9, 2, 2, &SET_n_r}, { "SET 3, D", 0xDA, 2, 2, &SET_n_r}, { "SET 3, E", 0xDB, 2, 2, &SET_n_r}, { "SET 3, H", 0xDC, 2, 2, &SET_n_r}, { "SET 3, L", 0xDD, 2, 2, &SET_n_r}, { "SET 3, (HL)", 0xDE, 2, 4, &SET_n_ind_HL}, { "SET 3, A", 0xDF, 2, 2, &SET_n_r}, 
/* 0xE+ */  { "SET 4, B", 0xE0, 2, 2, &SET_n_r}, { "SET 4, C", 0xE1, 2, 2, &SET_n_r}, { "SET 4, D", 0xE2, 2, 2, &SET_n_r}, { "SET 4, E", 0xE3, 2, 2, &SET_n_r}, { "SET 4, H", 0xE4, 2, 2, &SET_n_r}, { "SET 4, L", 0xE5, 2, 2, &SET_n_r}, { "SET 4, (HL)", 0xE6, 2, 4, &SET_n_ind_HL}, { "SET 4, A", 0xE7, 2, 2, &SET_n_r}, { "SET 5, B", 0xE8, 2, 2, &SET_n_r}, { "SET 5, C", 0xE9, 2, 2, &SET_n_r}, { "SET 5, D", 0xEA, 2, 2, &SET_n_r}, { "SET 5, E", 0xEB, 2, 2, &SET_n_r}, { "SET 5, H", 0xEC, 2, 2, &SET_n_r}, { "SET 5, L", 0xED, 2, 2, &SET_n_r}, { "SET 5, (HL)", 0xEE, 2, 4, &SET_n_ind_HL}, { "SET 5, A", 0xEF, 2, 2, &SET_n_r}, 
/* 0xF+ */  { "SET 6, B", 0xF0, 2, 2, &SET_n_r}, { "SET 0, 6", 0xF1, 2, 2, &SET_n_r}, { "SET 6, D", 0xF2, 2, 2, &SET_n_r}, { "SET 6, E", 0xF3, 2, 2, &SET_n_r}, { "SET 6, H", 0xF4, 2, 2, &SET_n_r}, { "SET 6, L", 0xF5, 2, 2, &SET_n_r}, { "SET 6, (HL)", 0xF6, 2, 4, &SET_n_ind_HL}, { "SET 6, A", 0xF7, 2, 2, &SET_n_r}, { "SET 7, B", 0xF8, 2, 2, &SET_n_r}, { "SET 7, C", 0xF9, 2, 2, &SET_n_r}, { "SET 7, D", 0xFA, 2, 2, &SET_n_r}, { "SET 7, E", 0xFB, 2, 2, &SET_n_r}, { "SET 7, H", 0xFC, 2, 2, &SET_n_r}, { "SET 7, L", 0xFD, 2, 2, &SET_n_r}, { "SET 7, (HL)", 0xFE, 2, 4, &SET_n_ind_HL}, { "SET 7, A", 0xFF, 2, 2, &SET_n_r}, 
};