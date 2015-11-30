#ifndef CPUMACROS_H
#define CPUMACROS_H

//#include "cpu.h"
//gbemu_cpu_t CPU;

#define GB_READ_U8(addr)         GB.MEMORY[addr]
#define GB_READ_S8(addr)         GB.sMEMORY[addr]
#define GB_WRITE_U8(addr, val)   GB.MEMORY[addr] = val

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
#define CPU_FLAG_NZ !CPU.FZ

#define CPU_FLAG_N  CPU.FN

#define CPU_FLAG_H  CPU.FH

#define CPU_FLAG_C  CPU.C
#define CPU_FLAG_NC !CPU.C

#define CPU_FLAG_ALWAYS 1

#define CPU_cycles_inc()      CPU.cycles ++
#define CPU_cycles_add(count) CPU.cycles += count
#define CPU_exec_next()       goto next_instruction

#define CPU_LD_r_imm8(reg) \
   reg = GB_READ_U8(REG_PC++);\
   CPU_cycles_add(2);\
   CPU_exec_next()

#define CPU_LD_rr_imm16(reg) \
   do{\
      uint16_t val = GB_READ_U8(REG_PC++);\
      val |= GB_READ_U8(REG_PC++) << 8;\
      reg = val;\
      CPU_cycles_add(3);\
      CPU_exec_next();\
   }while(0)

#define CPU_LD_addr16_r(reg, cycles) \
   do{\
      uint16_t addr = GB_READ_U8(REG_PC++);\
      addr |= GB_READ_U8(REG_PC++) << 8;\
      GB_WRITE_U8(addr, reg);\
      CPU_cycles_add(cycles);\
      CPU_exec_next();\
   }while(0)

#define CPU_LD_addr16_SP() CPU_LD_addr16_r(REG_SP, 5)


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


#define CPU_JP_addr8(cond) \
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
   }while(0)

/* ALU */

//#define CPU_ADD_r_r(reg0, reg1) \
//   do{\
//      uint16_t val = reg1;\
//      unsigned sum = reg0 + val;\
//      CPU.FH = (reg0 ^ val ^ sum) >> 4;\
//      reg0   = sum;\
//      CPU.FZ = !reg0;\
//      CPU.FN = 0;\
//      CPU.FC = sum >> 8;\
//      CPU_cycles_inc();\
//      CPU_exec_next();\
//   }while(0)

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


/* MISC */
#define CPU_NOP() \
   CPU_cycles_inc();\
   CPU_exec_next()

/* incomplete : */

#define CPU_STOP() \
   CPU_cycles_inc();\
   CPU_exec_next()


#endif // CPUMACROS_H
