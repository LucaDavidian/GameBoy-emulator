#if !defined __INSTRUCTION_SET_H__
#define __INSTRUCTION_SET_H__

#include <stdint.h>

typedef struct CPU CPU;

typedef void (*InstructionHandler)(CPU*);

typedef struct Instruction
{
    const char *name;                        // instruction name (debug)
    uint8_t opcode;                          // opcode (debug)
    uint8_t length;                          // instruction's byte length (debug)
    uint8_t machine_cycles;                  // total instruction's machine cycles
    InstructionHandler instruction_handler;  // instruction's microcode
} Instruction;

extern Instruction instruction_table[];
extern Instruction extended_instruction_table[];
extern Instruction interrupt;
extern Instruction halt_exit;

/******** Sharp LR35902 instruction set ********/
  
// 8-bit load instructions:
//  ld   r, r'           ----  r = r' (r and r' can be B, C, D, E, H, L, A)
//  ld   r, (HL)         ----  r = (HL)
//  ld   (HL), r         ----  (HL) = r
//  ld   r, u8           ----  r = u8
//  ld   (HL), u8        ----  (HL) = u8
//  ld   A, (BC)         ----  A = (BC)
//  ld   A, (DE)         ----  A = (DE)
//  ld   A, (u16)        ----  A = (u16)
//  ld   (BC), A         ----  (BC) = A
//  ld   (DE), A         ----  (DE) = A
//  ld   (u16), A        ----  (u16) = A
//  ld   A, (FF00 + u8)  ----  read from io-port u8 (memory FF00 + u8)
//  ld   (FF00 + u8), A  ----  write to io-port u8 (memory FF00 + u8)
//  ld   A, (FF00 + C)   ----  read from io-port C (memory FF00 + C)
//  ld   (FF00 + C), A   ----  write to io-port C (memory FF00 + C)
//  ldi  (HL), A         ----  (HL) = A, HL = HL + 1
//  ldi  A, (HL)         ----  A = (HL), HL = HL + 1
//  ldd  (HL), A         ----  (HL) = A, HL = HL - 1
//  ldd  A, (HL)         ----  A = (HL), HL = HL - 1

void LD_r1_r2(CPU*); 
void LD_r_ind_HL(CPU*);
void LD_ind_HL_r(CPU*);
void LD_r_u8(CPU*);
void LD_ind_HL_u8(CPU*);
void LD_A_ind_BC(CPU*);
void LD_A_ind_DE(CPU*);
void LD_A_ind_u16(CPU*);
void LD_ind_u16_A(CPU*);
void LD_ind_BC_A(CPU*);
void LD_ind_DE_A(CPU*);
void LD_A_ind_FF00_plus_u8(CPU*);
void LD_ind_FF00_plus_u8_A(CPU*);
void LD_A_ind_FF00_plus_C(CPU*);
void LD_ind_FF00_plus_C_A(CPU*);
void LDI_ind_HL_A(CPU*);
void LDI_A_ind_HL(CPU*);
void LDD_ind_HL_A(CPU*);
void LDD_A_ind_HL(CPU*);

// 16-bit load instructions:
//  ld   rr, u16    ----  rr = u16 (rr may be BC, DE, HL or SP)
//  ld   (u16), SP  ----  (u16) = SP
//  ld   SP, HL     ----  SP = HL
//  push rr         ----  SP = SP - 2  (SP) = rr   (rr may be BC, DE, HL, AF)
//  pop  rr         ----  rr = (SP)  SP = SP + 2   (rr may be BC, DE, HL, AF)

void LD_rr_u16(CPU*);
void LD_ind_u16_SP(CPU*);
void LD_SP_HL(CPU*);
void LD_HL_SP_plus_i8(CPU*);
void PUSH_rr(CPU*);
void POP_rr(CPU*);

// 8-bit arithmetic/logical instructions (r can be B, C, D, E, H, L, A):
//  add  A, r     z0hc  A = A + r 
//  add  A, (HL)  z0hc  A = A + (HL)
//  add  A, u8    z0hc  A = A + u8
//  adc  A, r     z0hc  A = A + r + cy
//  adc  A, (HL)  z0hc  A = A + (HL) + cy
//  adc  A, u8    z0hc  A = A + u8 + cy
//  sub  A, r     z1hc  A = A - r
//  sub  A, (HL)  z1hc  A = A - (HL)
//  sub  A, u8    z1hc  A = A - u8
//  sbc  A, r     z1hc  A = A - r - cy
//  sbc  A, (HL)  z1hc  A = A - (HL) - cy
//  sbc  A, u8    z1hc  A = A - u8 - cy
//  and  r        z010  A = A & r
//  and  (HL)     z010  A = A & (HL)
//  and  u8       z010  A = A & u8
//  xor  r        z000  A = A xor r
//  xor  (HL)     z000  A = A xor (HL)
//  xor  u8       z000  A = A xor u8
//  or   r        z000  A = A | r
//  or   (HL)     z000  A = A | (HL)
//  or   u8       z000  A = A | u8
//  cp   r        z1hc  compare A - r
//  cp   (HL)     z1hc  compare A - (HL)
//  cp   u8       z1hc  compare A - u8
//  inc  r        z0h-  r = r + 1
//  inc  (HL)     z0h-  (HL) = (HL) + 1
//  dec  r        z1h-  r = r - 1
//  dec  (HL)     z1h-  (HL) = (HL) - 1
//  daa           z-0x  decimal adjust akku
//  cpl           -11-  A = A xor FF

void ADD_A_r(CPU*);
void ADD_A_u8(CPU*);
void ADD_A_ind_HL(CPU*);
void ADC_A_r(CPU*);
void ADC_A_u8(CPU*);
void ADC_A_ind_HL(CPU*);
void SUB_A_r(CPU*);
void SUB_A_u8(CPU*);
void SUB_A_ind_HL(CPU*);
void SBC_A_r(CPU*);
void SBC_A_u8(CPU*);
void SBC_A_ind_HL(CPU*);
void AND_A_r(CPU*);
void AND_A_u8(CPU*);
void AND_A_ind_HL(CPU*);
void XOR_A_r(CPU*);
void XOR_A_u8(CPU*);
void XOR_A_ind_HL(CPU*);
void OR_A_r(CPU*);
void OR_A_u8(CPU*);
void OR_A_ind_HL(CPU*);
void CP_A_r(CPU*);
void CP_A_u8(CPU*);
void CP_A_ind_HL(CPU*);
void INC_r(CPU*);
void INC_ind_HL(CPU*);
void DEC_r(CPU*);
void DEC_ind_HL(CPU*);
void DAA(CPU*);
void CPL(CPU*);

//  16-bit arithmetic/logical instructions:
//  add  HL, rr       -0hc  HL = HL + rr     rr may be BC, DE, HL, SP
//  inc  rr           ----  rr = rr + 1      rr may be BC, DE, HL, SP
//  dec  rr           ----  rr = rr - 1      rr may be BC, DE, HL, SP
//  add  SP, dd       00hc  SP = SP +/- i8   i8 is 8-bit signed number
//  ld   HL, SP + i8  00hc  HL = SP +/- i8   i8 is 8-bit signed number

void INC_rr(CPU*);
void DEC_rr(CPU*);
void ADD_HL_rr(CPU*);
void ADD_SP_i8(CPU*);

//  rotate and shift instructions:
//  rlca        000c  rotate akku left
//  rla         000c  rotate akku left through carry
//  rrca        000c  rotate akku right
//  rra         000c  rotate akku right through carry
//  rlc  r      z00c  rotate left
//  rlc  (HL)   z00c  rotate left
//  rl   r      z00c  rotate left through carry
//  rl   (HL)   z00c  rotate left through carry
//  rrc  r      z00c  rotate right
//  rrc  (HL)   z00c  rotate right
//  rr   r      z00c  rotate right through carry
//  rr   (HL)   z00c  rotate right through carry
//  sla  r      z00c  shift left arithmetic (b0=0)
//  sla  (HL)   z00c  shift left arithmetic (b0=0)
//  swap r      z000  exchange low/hi-nibble
//  swap (HL)   z000  exchange low/hi-nibble
//  sra  r      z00c  shift right arithmetic (b7=b7)
//  sra  (HL)   z00c  shift right arithmetic (b7=b7)
//  srl  r      z00c  shift right logical (b7=0)
//  srl  (HL)   z00c  shift right logical (b7=0)

void RLA(CPU*);
void RRA(CPU*);
void RLCA(CPU*);
void RRCA(CPU*);
void RL_r(CPU*);
void RL_ind_HL(CPU*);
void RLC_r(CPU*);
void RLC_ind_HL(CPU*);
void RR_r(CPU*);
void RR_ind_HL(CPU*);
void RRC_r(CPU*);
void RRC_ind_HL(CPU*);
void SLA_r(CPU*);
void SLA_ind_HL(CPU*);
void SRA_r(CPU*);
void SRA_ind_HL(CPU*);
void SRL_r(CPU*);
void SRL_ind_HL(CPU*);
void SWAP_r(CPU*);
void SWAP_ind_HL(CPU*);

//  CPU control instructions:
//  ccf    -00c  cy = cy xor 1
//  scf    -001  cy = 1
//  nop    ----  no operation
//  halt   ----  halt until interrupt occurs (low power)
//  stop   ----  low power standby mode (VERY low power)
//  di     ----  disable interrupts, IME = 0
//  ei     ----  enable interrupts, IME = 1

void CCF(CPU*);
void SCF(CPU*);
void NOP(CPU*);
void HALT(CPU*);
void STOP(CPU*);
void DI(CPU*);
void EI(CPU*);

//  jump instructions:
//  jp   u16          ----  jump to nn, PC = u16
//  jp   HL           ----  jump to HL, PC = HL
//  jp   cc, nn       ----  conditional jump if nz, z, nc, c
//  jr   PC + i8      ----  relative jump to i8 (i8 8-bit signed)
//  jr   cc, PC + i8  ----  conditional relative jump if nz, z, nc, c
//  call u16          ----  call to nn, SP = SP - 2, (SP) = PC, PC = u16
//  call cc, u16      ----  conditional call if nz, z, nc, c
//  ret               ----  return, PC = (SP), SP = SP + 2
//  ret  cc           ----  conditional return if nz, z, nc, c
//  reti              ----  return and enable interrupts (IME = 1)
//  rst  n            ----  call to $00, $08, $10, $18, $20, $28, $30, $38

void JP_u16(CPU*);
void JP_HL(CPU*);
void JP_cc_u16(CPU*);
void JR_i8(CPU*);
void JR_cc_i8(CPU*);
void CALL_u16(CPU*);
void CALL_cc_u16(CPU*);
void RET(CPU*);
void RET_cc(CPU*);
void RETI(CPU*);
void RST_n(CPU*);

// single-bit operation instructions:
//  bit  n, r     z01-  test bit n
//  bit  n, (HL)  z01-  test bit n
//  set  n, r     ----  set bit n
//  set  n, (HL)  ----  set bit n
//  res  n, r     ----  reset bit n
//  res  n, (HL)  ----  reset bit n

void BIT_n_r(CPU*);
void BIT_n_ind_HL(CPU*);
void SET_n_r(CPU*);
void SET_n_ind_HL(CPU*);
void RES_n_r(CPU*);
void RES_n_ind_HL(CPU*);

#endif  // __INSTRUCTION_SET_H__