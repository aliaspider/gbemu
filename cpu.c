#include "gbemu.h"
#include "cpumacros.h"
#include "cpudisasm.h"

#include <stdint.h>
#include <signal.h>


void gbemu_cpu_run(void)
{
   gbemu_cpu_t CPU = GB.CPU;
   
next_instruction:
   gbemu_dump_state(&CPU);
   gbemu_disasm_current(&CPU);

   switch (GB.MEMORY[CPU.PC++])
   {
   // NOP
   //   case 0x7F:
   //   case 0x40:
   //   case 0x49:
   //   case 0x52:
   //   case 0x5B:
   //   case 0x64:
   //   case 0x6D:
   case 0x00:
      CPU_NOP();
   case 0x08:
      CPU_LD_addr16_SP();
   case 0x10:
      CPU_STOP();
   case 0x18:
      CPU_JP_addr8(CPU_FLAG_ALWAYS);
   case 0x20:
      CPU_JP_addr8(CPU_FLAG_NZ);
   case 0x28:
      CPU_JP_addr8(CPU_FLAG_Z);
   case 0x30:
      CPU_JP_addr8(CPU_FLAG_NC);
   case 0x38:
      CPU_JP_addr8(CPU_FLAG_C);

   case 0x01:
      CPU_LD_rr_imm16(REG_BC);
   case 0x11:
      CPU_LD_rr_imm16(REG_DE);
   case 0x21:
      CPU_LD_rr_imm16(REG_HL);
   case 0x31:
      CPU_LD_rr_imm16(REG_SP);

   case 0x09:
      CPU_ADD_rr_rr(REG_HL, REG_BC);
   case 0x19:
      CPU_ADD_rr_rr(REG_HL, REG_DE);
   case 0x29:
      CPU_ADD_rr_rr(REG_HL, REG_HL);
   case 0x39:
      CPU_ADD_rr_rr(REG_HL, REG_SP);

   case 0x02:
      CPU_LD_raddr_r(REG_BC, REG_A);
   case 0x0A:
      CPU_LD_r_raddr(REG_A, REG_BC);
   case 0x12:
      CPU_LD_raddr_r(REG_DE, REG_A);
   case 0x1A:
      CPU_LD_r_raddr(REG_A, REG_DE);
   case 0x22:
      CPU_LD_raddr_r(REG_HL++, REG_A);
   case 0x2A:
      CPU_LD_r_raddr(REG_A, REG_HL++);
   case 0x32:
      CPU_LD_raddr_r(REG_HL--, REG_A);
   case 0x3A:
      CPU_LD_r_raddr(REG_A, REG_HL--);

   case 0x03:
      CPU_INC_rr(REG_BC);
   case 0x0B:
      CPU_DEC_rr(REG_BC);
   case 0x13:
      CPU_INC_rr(REG_DE);
   case 0x1B:
      CPU_DEC_rr(REG_DE);
   case 0x23:
      CPU_INC_rr(REG_HL);
   case 0x2B:
      CPU_DEC_rr(REG_HL);
   case 0x33:
      CPU_INC_rr(REG_SP);
   case 0x3B:
      CPU_DEC_rr(REG_SP);

   case 0x04:
      CPU_INC_r(REG_B);
   case 0x0C:
      CPU_INC_r(REG_C);
   case 0x14:
      CPU_INC_r(REG_D);
   case 0x1C:
      CPU_INC_r(REG_E);
   case 0x24:
      CPU_INC_r(REG_H);
   case 0x2C:
      CPU_INC_r(REG_L);
   case 0x34:
      CPU_INC_raddr(REG_HL);
   case 0x3C:
      CPU_INC_r(REG_A);

   case 0x05:
      CPU_DEC_r(REG_B);
   case 0x0D:
      CPU_DEC_r(REG_C);
   case 0x15:
      CPU_DEC_r(REG_D);
   case 0x1D:
      CPU_DEC_r(REG_E);
   case 0x25:
      CPU_DEC_r(REG_H);
   case 0x2D:
      CPU_DEC_r(REG_L);
   case 0x35:
      CPU_DEC_raddr(REG_HL);
   case 0x3D:
      CPU_DEC_r(REG_A);

   case 0x06:
      CPU_LD_r_imm8(REG_B);
   case 0x0E:
      CPU_LD_r_imm8(REG_C);
   case 0x16:
      CPU_LD_r_imm8(REG_D);
   case 0x1E:
      CPU_LD_r_imm8(REG_E);
   case 0x26:
      CPU_LD_r_imm8(REG_H);
   case 0x2E:
      CPU_LD_r_imm8(REG_L);
   case 0x36:
      CPU_LD_raddr_imm8(REG_HL);
   case 0x3E:
      CPU_LD_r_imm8(REG_A);

   case 0x07:
      CPU_RLCA();
   case 0x0F:
      CPU_RRCA();
   case 0x17:
      CPU_RLA();
   case 0x1F:
      CPU_RRA();
   case 0x27:
      CPU_DAA();
   case 0x2F:
      CPU_CPL();
   case 0x37:
      CPU_SCF();
   case 0x3F:
      CPU_CCF();
      
   // LD r1, r2
   case 0x40:
      CPU_LD_r0_r1(CPU.B, CPU.B);
   case 0x41:
      CPU_LD_r0_r1(CPU.B, CPU.C);
   case 0x42:
      CPU_LD_r0_r1(CPU.B, CPU.D);
   case 0x43:
      CPU_LD_r0_r1(CPU.B, CPU.E);
   case 0x44:
      CPU_LD_r0_r1(CPU.B, CPU.H);
   case 0x45:
      CPU_LD_r0_r1(CPU.B, CPU.L);
   case 0x46:
      CPU_LD_r_raddr(CPU.B, CPU.HL);
   case 0x47:
      CPU_LD_r0_r1(CPU.B, CPU.A);

   case 0x48:
      CPU_LD_r0_r1(CPU.C, CPU.B);
   case 0x49:
      CPU_LD_r0_r1(CPU.C, CPU.C);
   case 0x4A:
      CPU_LD_r0_r1(CPU.C, CPU.D);
   case 0x4B:
      CPU_LD_r0_r1(CPU.C, CPU.E);
   case 0x4C:
      CPU_LD_r0_r1(CPU.C, CPU.H);
   case 0x4D:
      CPU_LD_r0_r1(CPU.C, CPU.L);
   case 0x4E:
      CPU_LD_r_raddr(CPU.C, CPU.HL);
   case 0x4F:
      CPU_LD_r0_r1(CPU.C, CPU.A);

   case 0x50:
      CPU_LD_r0_r1(CPU.D, CPU.B);
   case 0x51:
      CPU_LD_r0_r1(CPU.D, CPU.C);
   case 0x52:
      CPU_LD_r0_r1(CPU.D, CPU.D);
   case 0x53:
      CPU_LD_r0_r1(CPU.D, CPU.E);
   case 0x54:
      CPU_LD_r0_r1(CPU.D, CPU.H);
   case 0x55:
      CPU_LD_r0_r1(CPU.D, CPU.L);
   case 0x56:
      CPU_LD_r_raddr(CPU.D, CPU.HL);
   case 0x57:
      CPU_LD_r0_r1(CPU.D, CPU.A);

   case 0x58:
      CPU_LD_r0_r1(CPU.E, CPU.B);
   case 0x59:
      CPU_LD_r0_r1(CPU.E, CPU.C);
   case 0x5A:
      CPU_LD_r0_r1(CPU.E, CPU.D);
   case 0x5B:
      CPU_LD_r0_r1(CPU.E, CPU.E);
   case 0x5C:
      CPU_LD_r0_r1(CPU.E, CPU.H);
   case 0x5D:
      CPU_LD_r0_r1(CPU.E, CPU.L);
   case 0x5E:
      CPU_LD_r_raddr(CPU.E, CPU.HL);
   case 0x5F:
      CPU_LD_r0_r1(CPU.E, CPU.A);

   case 0x60:
      CPU_LD_r0_r1(CPU.H, CPU.B);
   case 0x61:
      CPU_LD_r0_r1(CPU.H, CPU.C);
   case 0x62:
      CPU_LD_r0_r1(CPU.H, CPU.D);
   case 0x63:
      CPU_LD_r0_r1(CPU.H, CPU.E);
   case 0x64:
      CPU_LD_r0_r1(CPU.H, CPU.H);
   case 0x65:
      CPU_LD_r0_r1(CPU.H, CPU.L);
   case 0x66:
      CPU_LD_r_raddr(CPU.H, CPU.HL);
   case 0x67:
      CPU_LD_r0_r1(CPU.H, CPU.A);

   case 0x68:
      CPU_LD_r0_r1(CPU.L, CPU.B);
   case 0x69:
      CPU_LD_r0_r1(CPU.L, CPU.C);
   case 0x6A:
      CPU_LD_r0_r1(CPU.L, CPU.D);
   case 0x6B:
      CPU_LD_r0_r1(CPU.L, CPU.E);
   case 0x6C:
      CPU_LD_r0_r1(CPU.L, CPU.H);
   case 0x6D:
      CPU_LD_r0_r1(CPU.L, CPU.L);
   case 0x6E:
      CPU_LD_r_raddr(CPU.L, CPU.HL);
   case 0x6F:
      CPU_LD_r0_r1(CPU.L, CPU.A);

   case 0x70:
      CPU_LD_raddr_r(CPU.HL, CPU.B);
   case 0x71:
      CPU_LD_raddr_r(CPU.HL, CPU.C);
   case 0x72:
      CPU_LD_raddr_r(CPU.HL, CPU.D);
   case 0x73:
      CPU_LD_raddr_r(CPU.HL, CPU.E);
   case 0x74:
      CPU_LD_raddr_r(CPU.HL, CPU.H);
   case 0x75:
      CPU_LD_raddr_r(CPU.HL, CPU.L);
   case 0x76:
      CPU_HALT();
   case 0x77:
      CPU_LD_raddr_r(CPU.HL, CPU.A);

   case 0x78:
      CPU_LD_r0_r1(CPU.A, CPU.B);
   case 0x79:
      CPU_LD_r0_r1(CPU.A, CPU.C);
   case 0x7A:
      CPU_LD_r0_r1(CPU.A, CPU.D);
   case 0x7B:
      CPU_LD_r0_r1(CPU.A, CPU.E);
   case 0x7C:
      CPU_LD_r0_r1(CPU.A, CPU.H);
   case 0x7D:
      CPU_LD_r0_r1(CPU.A, CPU.L);
   case 0x7E:
      CPU_LD_r_raddr(CPU.A, CPU.HL);
   case 0x7F:
      CPU_LD_r0_r1(CPU.A, CPU.A);

   case 0xFA:
   {
      uint16_t addr = GB.MEMORY[CPU.PC++];
      addr |= GB.MEMORY[CPU.PC++] << 8;
      CPU.A = GB.MEMORY[addr];
      CPU.cycles += 3;
      goto next_instruction;
   }
   
   case 0xEA:
   {
      uint16_t addr = GB.MEMORY[CPU.PC++];
      addr |= GB.MEMORY[CPU.PC++] << 8;
      GB.MEMORY[addr] = CPU.A;
      CPU.cycles += 4;
      goto next_instruction;
   }
   // LD A,(C)
   case 0xF2:
      CPU.A = GB.MEMORY[CPU.C | 0xFF00];
      CPU.cycles += 2;
      goto next_instruction;
   // LD (C), A
   case 0xE2:
      GB.MEMORY[CPU.C | 0xFF00] = CPU.A;
      CPU.cycles += 2;
      goto next_instruction;
      
   // LD A,(d8)
   case 0xF0:
      CPU.A = GB.MEMORY[GB.MEMORY[CPU.PC++] | 0xFF00];
      CPU.cycles += 3;
      goto next_instruction;
   // LD (d8), A
   case 0xE0:
      GB.MEMORY[GB.MEMORY[CPU.PC++] | 0xFF00] = CPU.A;
      CPU.cycles += 3;
      goto next_instruction;
      
   // LD SP, HL
   case 0xF9:
      CPU.SP = CPU.HL;
      CPU.cycles += 2;
      goto next_instruction;
   // LD HL, SP+n
   case 0xF8:
   {
      int8_t offset = GB.sMEMORY[CPU.PC++];
      unsigned val = CPU.SP + offset;
      CPU.HL = (uint16_t)val;
      CPU.FZ = 0;
      CPU.FN = 0;
      CPU.FH = (CPU.SP ^ offset ^ val) >> 4;
      CPU.FC = val >> 16;
      CPU.cycles += 3;
      goto next_instruction;
   }
   // PUSH (rr)
   case 0xC5:
      GB.MEMORY[--CPU.SP] = CPU.B;
      GB.MEMORY[--CPU.SP] = CPU.C;
      CPU.cycles += 4;
      goto next_instruction;
   case 0xD5:
      GB.MEMORY[--CPU.SP] = CPU.D;
      GB.MEMORY[--CPU.SP] = CPU.E;
      CPU.cycles += 4;
      goto next_instruction;
   case 0xE5:
      GB.MEMORY[--CPU.SP] = CPU.H;
      GB.MEMORY[--CPU.SP] = CPU.L;
      CPU.cycles += 4;
      goto next_instruction;
   case 0xF5:
      GB.MEMORY[--CPU.SP] = CPU.A;
      GB.MEMORY[--CPU.SP] = CPU.F;
      CPU.cycles += 4;
      goto next_instruction;
   // POP (rr)
   case 0xC1:
      CPU.C = GB.MEMORY[CPU.SP++];
      CPU.B = GB.MEMORY[CPU.SP++];
      CPU.cycles += 3;
      goto next_instruction;
   case 0xD1:
      CPU.E = GB.MEMORY[CPU.SP++];
      CPU.D = GB.MEMORY[CPU.SP++];
      CPU.cycles += 3;
      goto next_instruction;
   case 0xE1:
      CPU.L = GB.MEMORY[CPU.SP++];
      CPU.H = GB.MEMORY[CPU.SP++];
      CPU.cycles += 3;
      goto next_instruction;
   case 0xF1:
      CPU.F = GB.MEMORY[CPU.SP++];
      CPU.A = GB.MEMORY[CPU.SP++];
      CPU.cycles += 3;
      goto next_instruction;
      
   /* ALU */
   case 0x80:
      CPU_ADD_r_r(REG_A, REG_B);
   case 0x81:
      CPU_ADD_r_r(REG_A, REG_C);
   case 0x82:
      CPU_ADD_r_r(REG_A, REG_D);
   case 0x83:
      CPU_ADD_r_r(REG_A, REG_E);
   case 0x84:
      CPU_ADD_r_r(REG_A, REG_H);
   case 0x85:
      CPU_ADD_r_r(REG_A, REG_L);
   case 0x86:
      CPU_ADD_r_raddr(REG_A, REG_HL);
   case 0x87:
      CPU_ADD_r_r(REG_A, REG_A);

   case 0x88:
      CPU_ADC_r_r(REG_A, REG_B);
   case 0x89:
      CPU_ADC_r_r(REG_A, REG_C);
   case 0x8A:
      CPU_ADC_r_r(REG_A, REG_D);
   case 0x8B:
      CPU_ADC_r_r(REG_A, REG_E);
   case 0x8C:
      CPU_ADC_r_r(REG_A, REG_H);
   case 0x8D:
      CPU_ADC_r_r(REG_A, REG_L);
   case 0x8E:
      CPU_ADC_r_raddr(REG_A, REG_HL);
   case 0x8F:
      CPU_ADC_r_r(REG_A, REG_A);

   case 0x90:
      CPU_SUB_r_r(REG_A, REG_B);
   case 0x91:
      CPU_SUB_r_r(REG_A, REG_C);
   case 0x92:
      CPU_SUB_r_r(REG_A, REG_D);
   case 0x93:
      CPU_SUB_r_r(REG_A, REG_E);
   case 0x94:
      CPU_SUB_r_r(REG_A, REG_H);
   case 0x95:
      CPU_SUB_r_r(REG_A, REG_L);
   case 0x96:
      CPU_SUB_r_raddr(REG_A, REG_HL);
   case 0x97:
      CPU_SUB_r_r(REG_A, REG_A);

   case 0x98:
      CPU_SBC_r_r(REG_A, REG_B);
   case 0x99:
      CPU_SBC_r_r(REG_A, REG_C);
   case 0x9A:
      CPU_SBC_r_r(REG_A, REG_D);
   case 0x9B:
      CPU_SBC_r_r(REG_A, REG_E);
   case 0x9C:
      CPU_SBC_r_r(REG_A, REG_H);
   case 0x9D:
      CPU_SBC_r_r(REG_A, REG_L);
   case 0x9E:
      CPU_SBC_r_raddr(REG_A, REG_HL);
   case 0x9F:
      CPU_SBC_r_r(REG_A, REG_A);

   case 0xA0:
      CPU_AND_A_r(REG_B);
   case 0xA1:
      CPU_AND_A_r(REG_C);
   case 0xA2:
      CPU_AND_A_r(REG_D);
   case 0xA3:
      CPU_AND_A_r(REG_E);
   case 0xA4:
      CPU_AND_A_r(REG_H);
   case 0xA5:
      CPU_AND_A_r(REG_L);
   case 0xA6:
      CPU_AND_A_raddr(REG_HL);
   case 0xA7:
      CPU_AND_A_r(REG_A);

   case 0xA8:
      CPU_XOR_A_r(REG_B);
   case 0xA9:
      CPU_XOR_A_r(REG_C);
   case 0xAA:
      CPU_XOR_A_r(REG_D);
   case 0xAB:
      CPU_XOR_A_r(REG_E);
   case 0xAC:
      CPU_XOR_A_r(REG_H);
   case 0xAD:
      CPU_XOR_A_r(REG_L);
   case 0xAE:
      CPU_XOR_A_raddr(REG_HL);
   case 0xAF:
      CPU_XOR_A_r(REG_A);

   case 0xB0:
      CPU_OR_A_r(REG_B);
   case 0xB1:
      CPU_OR_A_r(REG_C);
   case 0xB2:
      CPU_OR_A_r(REG_D);
   case 0xB3:
      CPU_OR_A_r(REG_E);
   case 0xB4:
      CPU_OR_A_r(REG_H);
   case 0xB5:
      CPU_OR_A_r(REG_L);
   case 0xB6:
      CPU_OR_A_raddr(REG_HL);
   case 0xB7:
      CPU_OR_A_r(REG_A);

   case 0xB8:
      CPU_CP_A_r(REG_B);
   case 0xB9:
      CPU_CP_A_r(REG_C);
   case 0xBA:
      CPU_CP_A_r(REG_D);
   case 0xBB:
      CPU_CP_A_r(REG_E);
   case 0xBC:
      CPU_CP_A_r(REG_H);
   case 0xBD:
      CPU_CP_A_r(REG_L);
   case 0xBE:
      CPU_CP_A_raddr(REG_HL);
   case 0xBF:
      CPU_CP_A_r(REG_A);

   /*******/

   case 0xC6:
   {
      uint8_t val = GB.MEMORY[CPU.PC++];
      unsigned sum = CPU.A + val;
      CPU.FH = (CPU.A ^ val ^ sum) >> 4;
      CPU.A = sum;
      CPU.FZ = !CPU.A;
      CPU.FN = 0;
      CPU.FC = sum >> 8;
      CPU.cycles += 2;
      goto next_instruction;
   }

   case 0xCE:
   {
      uint8_t val = GB.MEMORY[CPU.PC++] + CPU.FC;
      unsigned sum = CPU.A + val;
      CPU.FH = (CPU.A ^ val ^ sum) >> 4;
      CPU.A = sum;
      CPU.FZ = !CPU.A;
      CPU.FN = 0;
      CPU.FC = sum >> 8;
      CPU.cycles += 2;
      goto next_instruction;
   }

   case 0xD6:
   {
      uint8_t val = GB.MEMORY[CPU.PC++];
      unsigned sum = CPU.A + val;
      CPU.FH = (CPU.A ^ val ^ sum) >> 4;
      CPU.A = sum;
      CPU.FZ = !CPU.A;
      CPU.FN = 0;
      CPU.FC = sum >> 8;
      CPU.cycles += 2;
      goto next_instruction;
   }

   case 0xDE:
   {
      uint8_t val = GB.MEMORY[CPU.PC++] + CPU.FC;
      unsigned sum = CPU.A + val;
      CPU.FH = (CPU.A ^ val ^ sum) >> 4;
      CPU.A = sum;
      CPU.FZ = !CPU.A;
      CPU.FN = 0;
      CPU.FC = sum >> 8;
      CPU.cycles += 2;
      goto next_instruction;
   }


   
   default:
      retro_sleep(10);
      printf("unknown opcode : 0x%02X\n", GB.MEMORY[CPU.PC - 1]);
      fflush(stdout);
      raise(SIGINT);
      exit(0);
      break;
   }
   
   GB.CPU = CPU;
}
