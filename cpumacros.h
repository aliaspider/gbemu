#ifndef CPUMACROS_H
#define CPUMACROS_H

//#include "cpu.h"
//gbemu_cpu_t CPU;

#if 0
#define GB_READ_U8(addr)         GB.MEMORY[addr]
#define GB_READ_S8(addr)         GB.sMEMORY[addr]
#define GB_WRITE_U8(addr, val)   GB.MEMORY[addr] = val
#else
#define GB_READ_U8(addr)         gbemu_read_u8(addr)
#define GB_READ_S8(addr)         gbemu_read_s8(addr)
#define GB_WRITE_U8(addr, val)   gbemu_write_u8(addr, val)
#endif

#define REG_A CPU.A
#define REG_F CPU.F
#define REG_B CPU.B
#define REG_C CPU.C
#define REG_D CPU.D
#define REG_E CPU.E
#define REG_H CPU.H
#define REG_L CPU.L
#define REG_BC CPU.BC
#define REG_DE CPU.DE
#define REG_HL CPU.HL
#define REG_SP CPU.SP
#define REG_PC CPU.PC

#define CPU_FLAG_Z  CPU.FZ
#define CPU_FLAG_N  CPU.FN
#define CPU_FLAG_H  CPU.FH
#define CPU_FLAG_C  CPU.FC

#define CPU_COND_Z  CPU.FZ
#define CPU_COND_NZ !CPU.FZ
#define CPU_COND_C  CPU.FC
#define CPU_COND_NC !CPU.FC
#define CPU_COND_ALWAYS 1

#define CPU_cycles_inc()      CPU.cycles++
#define CPU_cycles_add(count) CPU.cycles += count
#define CPU_exec_next()       goto next_instruction
#define CPU_exec_next_nocheck()       goto next_instruction_nocheck
#define CPU_enable_int()      CPU.IME = 1
#define CPU_disable_int()      CPU.IME = 0

#define CPU_LD_r_imm8(reg) \
   reg = GB_READ_U8(REG_PC++);\
   CPU_cycles_add(2);\
   CPU_exec_next()

#define CPU_LD_raddr_imm8(reg) \
   GB_WRITE_U8(reg, GB_READ_U8(REG_PC++));\
   CPU_cycles_add(3);\
   CPU_exec_next()

#define CPU_LD_rr_imm16(reg) \
   do{\
      uint16_t val = GB_READ_U8(REG_PC++);\
      val |= GB_READ_U8(REG_PC++) << 8;\
      reg = val;\
      CPU_cycles_add(3);\
      CPU_exec_next();\
   }while(0)

#define CPU_LD_addr16_A() \
   do{\
      uint16_t addr = GB_READ_U8(REG_PC++);\
      addr |= GB_READ_U8(REG_PC++) << 8;\
      GB_WRITE_U8(addr, REG_A);\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)


#define CPU_LD_addr16_SP() \
   do{\
      uint16_t addr = GB_READ_U8(REG_PC++);\
      addr |= GB_READ_U8(REG_PC++) << 8;\
      GB_WRITE_U8(addr, (REG_SP & 0xFF));\
      GB_WRITE_U8((addr + 1), (REG_SP >> 8));\
      CPU_cycles_add(5);\
      CPU_exec_next();\
   }while(0)

#define CPU_LD_r0_r1(reg0, reg1) \
   reg0 = reg1;\
   CPU_cycles_inc();\
   CPU_exec_next()

#define CPU_LD_r_raddr(reg, reg_addr) \
   reg = GB_READ_U8(reg_addr);\
   CPU_cycles_add(2);\
   CPU_exec_next()

#define CPU_LD_raddr_r(reg_addr, reg) \
   GB_WRITE_U8(reg_addr, reg);\
   CPU_cycles_add(2);\
   CPU_exec_next()

#define CPU_LD_r_addr8(reg) \
   reg = GB_READ_U8(GB_READ_U8(REG_PC++) | 0xFF00);\
   CPU_cycles_add(3);\
   CPU_exec_next();

#define CPU_LD_addr8_r(reg) \
   GB_WRITE_U8(GB_READ_U8(REG_PC++) | 0xFF00, reg);\
   CPU_cycles_add(3);\
   CPU_exec_next();

#define CPU_POP_rr(reg0, reg1) \
   reg0 = GB_READ_U8(REG_SP++);\
   reg1 = GB_READ_U8(REG_SP++);\
   CPU_cycles_add(3);\
   CPU_exec_next();

#define CPU_POP_BC() CPU_POP_rr(REG_C, REG_B)
#define CPU_POP_DE() CPU_POP_rr(REG_E, REG_D)
#define CPU_POP_HL() CPU_POP_rr(REG_L, REG_H)
#define CPU_POP_AF() CPU_POP_rr(REG_F, REG_A)

#define CPU_PUSH_rr(reg0, reg1) \
   GB_WRITE_U8(--REG_SP, reg0);\
   GB_WRITE_U8(--REG_SP, reg1);\
   CPU_cycles_add(4);\
   CPU_exec_next();

#define CPU_PUSH_BC() CPU_PUSH_rr(REG_B, REG_C)
#define CPU_PUSH_DE() CPU_PUSH_rr(REG_D, REG_E)
#define CPU_PUSH_HL() CPU_PUSH_rr(REG_H, REG_L)
#define CPU_PUSH_AF() CPU_PUSH_rr(REG_A, REG_F)


/* todo: correct C/H flag when off8 < 0 */
#define CPU_ADD_SP_off8() \
   do {\
      int8_t offset = GB_READ_S8(REG_PC++);\
      unsigned val = REG_SP + offset;\
      REG_HL = (uint16_t)val;\
      CPU_FLAG_Z = 0;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = (REG_SP ^ offset ^ val) >> 12;\
      CPU_FLAG_C = val >> 16;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)


/* todo: correct C/H flag when off8 < 0 */
#define CPU_LD_HL_SP_off8() \
   do {\
      int8_t offset = GB_READ_S8(REG_PC++);\
      unsigned val = REG_SP + offset;\
      CPU_FLAG_H = (REG_SP ^ offset ^ val) >> 12;\
      REG_SP = (uint16_t)val;\
      CPU_FLAG_Z = 0;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_C = val >> 16;\
      CPU_cycles_add(3);\
      CPU_exec_next();\
   }while(0)

/* ALU */

#define CPU_ADD_r_r(reg0, reg1) \
   do {\
   uint8_t val = reg1;\
   unsigned sum = reg0 + val;\
   CPU_FLAG_H = (reg0 ^ val ^ sum) >> 4;\
   reg0 = sum;\
   CPU_FLAG_Z = !reg0;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_C = sum >> 8;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_ADD_r_raddr(reg, reg_addr) \
   do {\
   uint8_t val = GB_READ_U8(reg_addr);\
   unsigned sum = reg + val;\
   CPU_FLAG_H = (reg ^ val ^ sum) >> 4;\
   reg = sum;\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_C = sum >> 8;\
   CPU_cycles_add(2);\
   CPU_exec_next();\
   }while(0)

#define CPU_ADD_r_imm8(reg) CPU_ADD_r_raddr(reg, REG_PC++)

#define CPU_ADC_r_r(reg0, reg1) \
   do {\
   uint8_t val = reg1 + CPU_FLAG_C;\
   unsigned sum = reg0 + val;\
   CPU_FLAG_H = (reg0 ^ val ^ sum) >> 4;\
   reg0 = sum;\
   CPU_FLAG_Z = !reg0;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_C = sum >> 8;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_ADC_r_raddr(reg, reg_addr) \
   do {\
   uint8_t val = GB_READ_U8(reg_addr) + CPU_FLAG_C;\
   unsigned sum = reg + val;\
   CPU_FLAG_H = (reg ^ val ^ sum) >> 4;\
   reg = sum;\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_C = sum >> 8;\
   CPU_cycles_add(2);\
   CPU_exec_next();\
   }while(0)

#define CPU_ADC_r_imm8(reg) CPU_ADC_r_raddr(reg, REG_PC++)

#define CPU_SUB_r_r(reg0, reg1) \
   do {\
   uint8_t val = reg1;\
   CPU_FLAG_H = (val & 0xF) > (reg0 & 0xF);\
   CPU_FLAG_C = val > reg0;\
   reg0 -= val;\
   CPU_FLAG_Z = !reg0;\
   CPU_FLAG_N = 1;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_SUB_r_raddr(reg, reg_addr) \
   do {\
   uint8_t val = GB_READ_U8(reg_addr);\
   CPU_FLAG_H = (val & 0xF) > (reg & 0xF);\
   CPU_FLAG_C = val > reg;\
   reg -= val;\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 1;\
   CPU_cycles_add(2);\
   CPU_exec_next();\
   }while(0)

#define CPU_SUB_r_imm8(reg) CPU_SUB_r_raddr(reg, REG_PC++)

#define CPU_SBC_r_r(reg0, reg1) \
   do {\
   uint8_t val = reg1 + CPU_FLAG_C;\
   CPU_FLAG_H = (val & 0xF) > (reg0 & 0xF);\
   CPU_FLAG_C = val > reg0;\
   reg0 -= val;\
   CPU_FLAG_Z = !reg0;\
   CPU_FLAG_N = 1;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_SBC_r_raddr(reg, reg_addr) \
   do {\
   uint8_t val = GB_READ_U8(reg_addr) + CPU_FLAG_C;\
   CPU_FLAG_H = (val & 0xF) > (reg & 0xF);\
   CPU_FLAG_C = val > reg;\
   reg -= val;\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 1;\
   CPU_cycles_add(2);\
   CPU_exec_next();\
   }while(0)

#define CPU_SBC_r_imm8(reg) CPU_SBC_r_raddr(reg, REG_PC++)

#define CPU_ADD_rr_rr(reg0, reg1) \
   do{\
      uint16_t val = reg1;\
      unsigned sum = reg0 + val;\
      CPU.FH = (reg0 ^ val ^ sum) >> 12;\
      reg0   = sum;\
      CPU.FN = 0;\
      CPU.FC = sum >> 16;\
      CPU_cycles_add(2);\
      CPU_exec_next();\
   }while(0)

#define CPU_AND_A_r(reg) \
   REG_A &= reg;\
   CPU_FLAG_Z = !REG_A;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 1;\
   CPU_FLAG_C = 0;\
   CPU_cycles_inc();\
   CPU_exec_next()

#define CPU_AND_A_raddr(reg_addr) \
   REG_A &= GB_READ_U8(reg_addr);\
   CPU_FLAG_Z = !REG_A;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 1;\
   CPU_FLAG_C = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_AND_A_imm8() CPU_AND_A_raddr(REG_PC++)

#define CPU_XOR_A_r(reg) \
   REG_A ^= reg;\
   CPU_FLAG_Z = !REG_A;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_XOR_A_raddr(reg_addr) \
   REG_A ^= GB_READ_U8(reg_addr);\
   CPU_FLAG_Z = !REG_A;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_XOR_A_imm8() CPU_XOR_A_raddr(REG_PC++)

#define CPU_OR_A_r(reg) \
   REG_A |= reg;\
   CPU_FLAG_Z = !REG_A;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_OR_A_raddr(reg_addr) \
   REG_A |= GB_READ_U8(reg_addr);\
   CPU_FLAG_Z = !REG_A;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_OR_A_imm8() CPU_OR_A_raddr(REG_PC++)

#define CPU_CP_A_r(reg) \
   do {\
   uint8_t val = reg;\
   CPU_FLAG_H = (val & 0xF) > (REG_A & 0xF);\
   CPU_FLAG_C = val > REG_A;\
   CPU_FLAG_Z = val == REG_A;\
   CPU_FLAG_N = 1;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_CP_A_raddr(reg_addr) \
   do {\
   uint8_t val = GB_READ_U8(reg_addr);\
   CPU_FLAG_H = (val & 0xF) > (REG_A & 0xF);\
   CPU_FLAG_C = val > REG_A;\
   CPU_FLAG_Z = val == REG_A;\
   CPU_FLAG_N = 1;\
   CPU_cycles_add(2);\
   CPU_exec_next();\
   }while(0)

#define CPU_CP_A_imm8() CPU_CP_A_raddr(REG_PC++)

#define CPU_INC_r(reg) \
   reg++;\
   CPU_cycles_inc();\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = !(reg & 0xF);\
   CPU_exec_next();

#define CPU_DEC_r(reg) \
   CPU_FLAG_H = !(reg & 0xF);\
   reg--;\
   CPU_cycles_inc();\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 1;\
   CPU_exec_next();

#define CPU_INC_raddr(reg) \
   do{\
   uint8_t val = GB_READ_U8(reg);\
   val++;\
   GB_WRITE_U8(reg, val);\
   CPU_cycles_add(3);\
   CPU_FLAG_Z = !val;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = !(val & 0xF);\
   CPU_exec_next();\
   }while(0)

#define CPU_DEC_raddr(reg) \
   do{\
   uint8_t val = GB_READ_U8(reg);\
   CPU_FLAG_H = !(val & 0xF);\
   val--;\
   GB_WRITE_U8(reg, val);\
   CPU_cycles_add(3);\
   CPU_FLAG_Z = !val;\
   CPU_FLAG_N = 1;\
   CPU_exec_next();\
   }while(0)

#define CPU_INC_rr(reg) \
   reg++;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_DEC_rr(reg) \
   reg--;\
   CPU_cycles_add(2);\
   CPU_exec_next();


#define CPU_RLCA() \
   REG_A = (REG_A << 1) | (REG_A >> 7);\
   CPU_FLAG_C = REG_A;\
   CPU_FLAG_Z = 0;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_RRCA() \
   CPU_FLAG_C = REG_A;\
   REG_A = (REG_A >> 1) | (REG_A << 7);\
   CPU_FLAG_Z = 0;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_RLA() \
   do {\
   unsigned val = (REG_A << 1) | CPU_FLAG_C;\
   REG_A = val;\
   CPU_FLAG_C = val >> 8;\
   CPU_FLAG_Z = 0;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_RRA() \
   do {\
   unsigned val = (REG_A >> 1) | (CPU_FLAG_C << 7);\
   CPU_FLAG_C = REG_A;\
   REG_A = val;\
   CPU_FLAG_Z = 0;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_inc();\
   CPU_exec_next();\
   }while(0)

#define CPU_DAA() \
   do{\
      unsigned low = REG_A & 0xF;\
      unsigned high = REG_A >> 4;\
      if(CPU_FLAG_N)\
      {\
         if(CPU_FLAG_H)\
         {\
            low -= 0x6;\
            high++;\
         }\
         if(CPU_FLAG_C)\
         {\
            high -= 0x6;\
         }\
      }\
      else\
      {\
         if(CPU_FLAG_H)\
         {\
            low += 0x6;\
            high--;\
         }\
         if(low > 0x9)\
         {\
            low -= 0x9;\
            high++;\
         }\
         if(CPU_FLAG_C)\
         {\
            high += 0x6;\
         }\
         if(high > 0x9)\
         {\
            high -= 0x9;\
            CPU_FLAG_C = 1;\
         }\
      }\
      REG_A = low | high << 4;\
      CPU_FLAG_Z = !REG_A;\
      CPU_FLAG_H = 0;\
      CPU_cycles_inc();\
      CPU_exec_next();\
   }while(0)


#define CPU_CPL() \
   REG_A ^= 0xFF;\
   CPU_FLAG_N = 1;\
   CPU_FLAG_H = 1;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_SCF() \
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 1;\
   CPU_cycles_inc();\
   CPU_exec_next();

#define CPU_CCF() \
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C ^= 1;\
   CPU_cycles_inc();\
   CPU_exec_next();

/* CB */
#define CPU_RLC(reg) \
   reg = (reg << 1) | (reg >> 7);\
   CPU_FLAG_C = reg;\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_RLC_HL() \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      val = (val << 1) | (val >> 7);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_C = val;\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)


#define CPU_RRC(reg) \
   CPU_FLAG_C = reg;\
   reg = (reg >> 1) | (reg << 7);\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_RRC_HL() \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      CPU_FLAG_C = val;\
      val = (val >> 1) | (val << 7);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_RL(reg) \
   do {\
      unsigned val = (reg << 1) | CPU_FLAG_C;\
      reg = val;\
      CPU_FLAG_C = val >> 8;\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(2);\
      CPU_exec_next();\
   }while(0)

#define CPU_RL_HL() \
   do {\
      unsigned val = (GB_READ_U8(REG_HL) << 1) | CPU_FLAG_C;\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_C = val >> 8;\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_RR(reg) \
   do {\
      unsigned val = (reg >> 1) | (CPU_FLAG_C << 7);\
      CPU_FLAG_C = reg;\
      reg = val;\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(2);\
      CPU_exec_next();\
   }while(0)

#define CPU_RR_HL() \
   do {\
      unsigned val = GB_READ_U8(REG_HL);\
      CPU_FLAG_C = val;\
      val = (val >> 1) | (CPU_FLAG_C << 7);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_SLA(reg) \
   CPU_FLAG_C = reg >> 7;\
   reg <<= 1;\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_SLA_HL() \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      CPU_FLAG_C = val >> 7;\
      GB_WRITE_U8(REG_HL, val << 1);\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_SRA(reg) \
   reg = (reg >> 1) | (reg & 0x80);\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 0;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_SRA_HL() \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      val = (val >> 1) | (val & 0x80);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_FLAG_C = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_SWAP(reg) \
   reg = (reg >> 4) | (reg << 4);\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_FLAG_C = 0;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_SWAP_HL() \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      val = (val >> 4) | (val << 4);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_FLAG_C = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_SRL(reg) \
   CPU_FLAG_C = reg;\
   reg = (reg >> 1);\
   CPU_FLAG_Z = !reg;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 0;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_SRL_HL() \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      CPU_FLAG_C = val;\
      val = (val >> 1);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_FLAG_Z = !val;\
      CPU_FLAG_N = 0;\
      CPU_FLAG_H = 0;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)


#define CPU_BIT(bit, reg) \
   CPU_FLAG_Z = (reg >> bit) ^ 1;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 1;\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_BIT_HL(bit) \
   CPU_FLAG_Z = (GB_READ_U8(REG_HL) >> bit) ^ 1;\
   CPU_FLAG_N = 0;\
   CPU_FLAG_H = 1;\
   CPU_cycles_add(4);\
   CPU_exec_next();

#define CPU_RES(bit, reg) \
   reg &= ~(1 << bit);\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_RES_HL(bit) \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      val &= ~(1 << bit);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_SET(bit, reg) \
   reg |= (1 << bit);\
   CPU_cycles_add(2);\
   CPU_exec_next();

#define CPU_SET_HL(bit) \
   do {\
      uint8_t val = GB_READ_U8(REG_HL);\
      val |= (1 << bit);\
      GB_WRITE_U8(REG_HL, val);\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)


/* control flow */
#define CPU_JR(cond) \
   do\
   {\
      if(cond)\
      {\
         REG_PC += GB_READ_S8(REG_PC) + 1;\
         CPU_cycles_add(3);\
      }\
      else\
      {\
         REG_PC++;\
         CPU_cycles_add(2);\
      }\
      CPU_exec_next();\
   }while(0)

#define CPU_JP(cond) \
   do\
   {\
      if(cond)\
      {\
         uint16_t addr = GB_READ_U8(REG_PC++);\
         addr |= GB_READ_U8(REG_PC++) << 8;\
         REG_PC = addr;\
         CPU_cycles_add(4);\
      }\
      else\
      {\
         REG_PC+=2;\
         CPU_cycles_add(3);\
      }\
      CPU_exec_next();\
   }while(0)

#define CPU_JP_HL() \
      REG_PC = REG_HL;\
      CPU_cycles_inc();\
      CPU_exec_next();

#define CPU_CALL(cond) \
   do\
   {\
      if(cond)\
      {\
         uint16_t addr = GB_READ_U8(REG_PC++);\
         addr |= GB_READ_U8(REG_PC++) << 8;\
         GB_WRITE_U8(--REG_SP, (uint8_t)(REG_PC >> 8));\
         GB_WRITE_U8(--REG_SP, (uint8_t)REG_PC);\
         REG_PC = addr;\
         CPU_cycles_add(6);\
      }\
      else\
      {\
         REG_PC+=2;\
         CPU_cycles_add(3);\
      }\
      CPU_exec_next();\
   }while(0)


#define CPU_RET_cc(cc) \
   do\
   {\
      if(cc)\
      {\
         uint16_t addr = GB_READ_U8(REG_SP++);\
         addr |= GB_READ_U8(REG_SP++) << 8;\
         REG_PC = addr;\
         CPU_cycles_add(5);\
      }\
      else\
         CPU_cycles_add(2);\
      CPU_exec_next();\
   }while(0)

#define CPU_RET() \
   do\
   {\
      uint16_t addr = GB_READ_U8(REG_SP++);\
      addr |= GB_READ_U8(REG_SP++) << 8;\
      REG_PC = addr;\
      CPU_cycles_add(4);\
      CPU_exec_next();\
   }while(0)

#define CPU_RETI() \
   do\
   {\
      uint16_t addr = GB_READ_U8(REG_SP++);\
      addr |= GB_READ_U8(REG_SP++) << 8;\
      REG_PC = addr;\
      CPU_cycles_add(4);\
      CPU_enable_int();\
      CPU_exec_next();\
   }while(0)


/* MISC */
#define CPU_NOP() \
   CPU_cycles_inc();\
   CPU_exec_next()

#define CPU_DI() \
   CPU_disable_int();\
   CPU_cycles_inc();\
   CPU_exec_next_nocheck()

#define CPU_EI() \
   CPU_enable_int();\
   CPU_cycles_inc();\
   CPU_exec_next_nocheck()

#define CPU_INT(addr) \
   do{\
   GB_WRITE_U8(--REG_SP, (uint8_t)(REG_PC >> 8));\
   GB_WRITE_U8(--REG_SP, (uint8_t)REG_PC);\
   REG_PC = addr;\
   CPU_cycles_add(5);\
   }while(0)


#define CPU_RST(addr) \
   do{\
   GB_WRITE_U8(--REG_SP, (uint8_t)(REG_PC >> 8));\
   GB_WRITE_U8(--REG_SP, (uint8_t)REG_PC);\
   REG_PC = addr;\
   CPU_cycles_add(4);\
   CPU_exec_next();\
   }while(0)


/* incomplete : */

#define CPU_HALT() \
   CPU_cycles_inc();\
   CPU_exec_next()

#define CPU_STOP() \
   CPU_cycles_inc();\
   CPU_exec_next()


#endif // CPUMACROS_H
