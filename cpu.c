#include "gbemu.h"
#include <stdint.h>
#include <signal.h>


void gbemu_cpu_run(void)
{
   gbemu_cpu_t CPU = GB.CPU;

next_instruction:
   switch (GB.MEMORY[CPU.PC++])
   {
   // NOP
   case 0x7F:
   case 0x40:
   case 0x49:
   case 0x52:
   case 0x5B:
   case 0x64:
   case 0x6D:
   case 0x00:
      CPU.cycles++;
      goto next_instruction;

   // LD r, d8
   case 0x06:
      CPU.B = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x0E:
      CPU.C = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x16:
      CPU.D = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x1E:
      CPU.E = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x26:
      CPU.H = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x2E:
      CPU.L = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x3E:
      CPU.A = GB.MEMORY[CPU.PC++];
      CPU.cycles += 2;
      goto next_instruction;

   // LD r1, r2
   case 0x78:
      CPU.A = CPU.B;
      CPU.cycles ++;
      goto next_instruction;
   case 0x79:
      CPU.A = CPU.C;
      CPU.cycles ++;
      goto next_instruction;
   case 0x7A:
      CPU.A = CPU.D;
      CPU.cycles ++;
      goto next_instruction;
   case 0x7B:
      CPU.A = CPU.E;
      CPU.cycles ++;
      goto next_instruction;
   case 0x7C:
      CPU.A = CPU.H;
      CPU.cycles ++;
      goto next_instruction;
   case 0x7D:
      CPU.A = CPU.L;
      CPU.cycles ++;
      goto next_instruction;
   case 0x7E:
      CPU.A = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;
//   case 0x7F:
//      CPU.A = CPU.A;
//      CPU.cycles ++;
//      goto next_instruction;

//   case 0x40:
//     CPU.B = CPU.B;
//     CPU.cycles ++;
//     goto next_instruction;
   case 0x41:
      CPU.B = CPU.C;
      CPU.cycles ++;
      goto next_instruction;
   case 0x42:
      CPU.B = CPU.D;
      CPU.cycles ++;
      goto next_instruction;
   case 0x43:
      CPU.B = CPU.E;
      CPU.cycles ++;
      goto next_instruction;
   case 0x44:
      CPU.B = CPU.H;
      CPU.cycles ++;
      goto next_instruction;
   case 0x45:
      CPU.B = CPU.L;
      CPU.cycles ++;
      goto next_instruction;
   case 0x46:
      CPU.B = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;

   case 0x48:
      CPU.C = CPU.B;
      CPU.cycles ++;
      goto next_instruction;
//   case 0x49:
//     CPU.C = CPU.C;
//     CPU.cycles ++;
//     goto next_instruction;
   case 0x4A:
      CPU.C = CPU.D;
      CPU.cycles ++;
      goto next_instruction;
   case 0x4B:
      CPU.C = CPU.E;
      CPU.cycles ++;
      goto next_instruction;
   case 0x4C:
      CPU.C = CPU.H;
      CPU.cycles ++;
      goto next_instruction;
   case 0x4D:
      CPU.C = CPU.L;
      CPU.cycles ++;
      goto next_instruction;
   case 0x4E:
      CPU.C = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;

   case 0x50:
      CPU.D = CPU.B;
      CPU.cycles ++;
      goto next_instruction;
   case 0x51:
     CPU.D = CPU.C;
     CPU.cycles ++;
     goto next_instruction;
//   case 0x52:
//      CPU.D = CPU.D;
//      CPU.cycles ++;
//      goto next_instruction;
   case 0x53:
      CPU.D = CPU.E;
      CPU.cycles ++;
      goto next_instruction;
   case 0x54:
      CPU.D = CPU.H;
      CPU.cycles ++;
      goto next_instruction;
   case 0x55:
      CPU.D = CPU.L;
      CPU.cycles ++;
      goto next_instruction;
   case 0x56:
      CPU.D = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;

   case 0x58:
      CPU.E = CPU.B;
      CPU.cycles ++;
      goto next_instruction;
   case 0x59:
      CPU.E = CPU.C;
      CPU.cycles ++;
      goto next_instruction;
   case 0x5A:
      CPU.E = CPU.D;
      CPU.cycles ++;
      goto next_instruction;
//   case 0x5B:
//      CPU.E = CPU.E;
//      CPU.cycles ++;
//      goto next_instruction;
   case 0x5C:
      CPU.E = CPU.H;
      CPU.cycles ++;
      goto next_instruction;
   case 0x5D:
      CPU.E = CPU.L;
      CPU.cycles ++;
      goto next_instruction;
   case 0x5E:
      CPU.E = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;


   case 0x60:
      CPU.H = CPU.B;
      CPU.cycles ++;
      goto next_instruction;
   case 0x61:
      CPU.H = CPU.C;
      CPU.cycles ++;
      goto next_instruction;
   case 0x62:
      CPU.H = CPU.D;
      CPU.cycles ++;
      goto next_instruction;
   case 0x63:
      CPU.H = CPU.E;
      CPU.cycles ++;
      goto next_instruction;
//   case 0x64:
//      CPU.H = CPU.H;
//      CPU.cycles ++;
//      goto next_instruction;
   case 0x65:
      CPU.H = CPU.L;
      CPU.cycles ++;
      goto next_instruction;
   case 0x66:
      CPU.H = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;

   case 0x68:
      CPU.L = CPU.B;
      CPU.cycles ++;
      goto next_instruction;
   case 0x69:
      CPU.L = CPU.C;
      CPU.cycles ++;
      goto next_instruction;
   case 0x6A:
      CPU.L = CPU.D;
      CPU.cycles ++;
      goto next_instruction;
   case 0x6B:
      CPU.L = CPU.E;
      CPU.cycles ++;
      goto next_instruction;
   case 0x6C:
      CPU.L = CPU.H;
      CPU.cycles ++;
      goto next_instruction;
//   case 0x6D:
//      CPU.L = CPU.L;
//      CPU.cycles ++;
//      goto next_instruction;
   case 0x6E:
      CPU.L = GB.MEMORY[CPU.HL];
      CPU.cycles += 2;
      goto next_instruction;

   case 0x70:
      GB.MEMORY[CPU.HL] = CPU.B;
      CPU.cycles += 2;
      goto next_instruction;
   case 0x71:
      GB.MEMORY[CPU.HL] = CPU.C;
      CPU.cycles += 2;
      goto next_instruction;
   case 0x72:
      GB.MEMORY[CPU.HL] = CPU.D;
      CPU.cycles += 2;
      goto next_instruction;
   case 0x73:
      GB.MEMORY[CPU.HL] = CPU.E;
      CPU.cycles += 2;
      goto next_instruction;
   case 0x74:
      GB.MEMORY[CPU.HL] = CPU.H;
      CPU.cycles += 2;
      goto next_instruction;
   case 0x75:
      GB.MEMORY[CPU.HL] = CPU.L;
      CPU.cycles += 2;
      goto next_instruction;
   case 0x36:
      GB.MEMORY[CPU.HL] = GB.MEMORY[CPU.PC++];
      CPU.cycles += 3;
      goto next_instruction;

   // LD A, (r)
   case 0x0A:
      CPU.A = GB.MEMORY[CPU.BC];
      CPU.cycles += 2;
      goto next_instruction;
   case 0x1A:
      CPU.A = GB.MEMORY[CPU.DE];
      CPU.cycles += 2;
      goto next_instruction;
   case 0xFA:
   {
      uint16_t addr = GB.MEMORY[CPU.PC++];
      addr |= GB.MEMORY[CPU.PC++] << 8;
      CPU.A = GB.MEMORY[addr];
      CPU.cycles += 4;
      goto next_instruction;
   }

   // LD r, A
   case 0x47:
      CPU.B = CPU.A;
      CPU.cycles ++;
      goto next_instruction;
   case 0x4F:
      CPU.C = CPU.A;
      CPU.cycles ++;
      goto next_instruction;
   case 0x57:
      CPU.D = CPU.A;
      CPU.cycles ++;
      goto next_instruction;
   case 0x5F:
      CPU.E = CPU.A;
      CPU.cycles ++;
      goto next_instruction;
   case 0x67:
      CPU.H = CPU.A;
      CPU.cycles ++;
      goto next_instruction;
   case 0x6F:
      CPU.L = CPU.A;
      CPU.cycles ++;
      goto next_instruction;

  // LD (rr), A
   case 0x02:
      GB.MEMORY[CPU.BC] = CPU.A;
      CPU.cycles += 2;
   case 0x12:
      GB.MEMORY[CPU.DE] = CPU.A;
      CPU.cycles += 2;
   case 0x77:
      GB.MEMORY[CPU.HL] = CPU.A;
      CPU.cycles += 2;
      goto next_instruction;
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
   // LD A, (HL-)
   case 0x3A:
      CPU.A = GB.MEMORY[CPU.HL--];
      CPU.cycles += 2;
      goto next_instruction;
   // LD (HL-), A
   case 0x32:
      GB.MEMORY[CPU.HL--] = CPU.A;
      CPU.cycles += 2;
      goto next_instruction;
   // LD A, (HL+)
   case 0x2A:
      CPU.A = GB.MEMORY[CPU.HL++];
      CPU.cycles += 2;
      goto next_instruction;
   // LD (HL+), A
   case 0x22:
      GB.MEMORY[CPU.HL++] = CPU.A;
      CPU.cycles += 2;
      goto next_instruction;

   // LD A,(d8)
   case 0xF0:
      CPU.A = GB.MEMORY[GB.MEMORY[CPU.PC++] | 0xFF00];
      CPU.cycles += 4;
      goto next_instruction;
   // LD (d8), A
   case 0xE0:
      GB.MEMORY[GB.MEMORY[CPU.PC++] | 0xFF00] = CPU.A;
      CPU.cycles += 4;
      goto next_instruction;



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
