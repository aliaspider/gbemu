#include <stdio.h>
#include "gbemu.h"
#include "cpu.h"
#include "cpudisasm.h"

void gbemu_disasm_current(gbemu_cpu_t* CPU)
{
   static const char* reg_names[] =
   {
      "B", "C", "D", "E", "H", "L", "(HL)", "A"
   };

   char immediate8[3];
   char immediate16[5];
   char immediate8_addr[5];
   char immediate16_addr[7];

   struct
   {
      union
      {
         struct
         {
            unsigned r1  : 3;
            unsigned r0  : 3;
            unsigned op0 : 2;
         };
         struct
         {
            unsigned op1 : 3;
            unsigned op2 : 1;
            unsigned rr  : 2;
            unsigned     : 2;

         };
         uint8_t val;
      };
      const char* label;
      const char* operand0;
      const char* operand1;
      char zero;
      char negative;
      char halfcarry;
      char carry;
      int cycles;
      int cycles2;
      int size;

   } op;

   op.val         = GB.MEMORY[CPU->PC];
   op.label       = NULL;
   op.operand0    = NULL;
   op.operand1    = NULL;
   op.zero        = '-';
   op.negative    = '-';
   op.halfcarry   = '-';
   op.carry       = '-';
   op.cycles      = 1;
   op.cycles2     = 2;
   op.size        = 1;


   snprintf(immediate8, sizeof(immediate8),"%02X", GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(immediate16, sizeof(immediate16),"%02X%02X", GB.MEMORY[(CPU->PC + 2) & 0xFFFF], GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(immediate8_addr, sizeof(immediate8_addr),"(%02X)", GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(immediate16_addr, sizeof(immediate16_addr),"(%02X%02X)", GB.MEMORY[(CPU->PC + 2) & 0xFFFF], GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);

   switch (op.val)
   {
   case 0x00:
      op.label = "NOP";
      break;
   case 0x08:
      op.label = "LD";
      op.operand0 = immediate16_addr;
      op.operand1 = "SP";
      break;
   case 0x10:
      op.label = "STOP";
      op.operand0 = immediate8;
      op.size++;
      break;
   case 0x13:
      op.label = "DI";
      break;
   case 0xCB:
      op.val = GB.MEMORY[(CPU->PC + 1) & 0xFFFF];
      op.size++;
      switch (op.val)
      {

      }
      break;

   default:
      switch (op.op0)
      {
      case 0b00:
         switch (op.op1)
         {
         case 0b000:
            op.label = "JR";
            op.size++;
            break;
         case 0b001:
            break;
         case 0b010:
            break;
         case 0b011:
            break;
         case 0b100:
            break;
         case 0b101:
            break;
         case 0b110:
            break;
         case 0b111:
            break;
         }
         break;
      case 0b01:
         op.label = "LD";
         if (op.r0 == 0b110 || op.r1 == 0b110)
            op.cycles++;
         op.operand0 = reg_names[op.r0];
         op.operand1 = reg_names[op.r1];
         break;
      case 0b10:
         break;
      case 0b11:
         break;
      }
      break;
   }

   if(op.size == 1)
      printf("%02X          :", GB.MEMORY[CPU->PC]);
   else if(op.size == 2)
      printf("%02X %02X     :", GB.MEMORY[CPU->PC], GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   else
      printf("%02X %02X %02X:", GB.MEMORY[CPU->PC], GB.MEMORY[(CPU->PC + 1) & 0xFFFF], GB.MEMORY[(CPU->PC + 2) & 0xFFFF]);

   if (op.label)
   {
      printf("+%i", op.cycles);
      if()
      printf("(1)(%c %c %c %c)  %s",
             op.size, op.zero, op.negative, op.halfcarry, op.carry,
             op.label);
      if (op.operand0)
         printf(" %s", op.operand0);
      if (op.operand1)
         printf(", %s", op.operand1);
   }
   else
      printf("+?  (?)(? ? ? ?)  ????");




   printf("\n");
   fflush(stdout);
}

void gbemu_dump_state(gbemu_cpu_t* CPU)
{

   printf("%8u PC: %04X SP: %04X AF: %04X BC: %04X DE: %04X HL: %04X (Z:%u N:%u H:%u C:%u)\n",
          CPU->cycles, CPU->PC, CPU->SP, CPU->AF, CPU->BC, CPU->DE, CPU->HL,
          CPU->FZ, CPU->FN, CPU->FH, CPU->FC);
   fflush(stdout);
}
