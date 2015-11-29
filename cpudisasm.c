#include <stdio.h>
#include "gbemu.h"
#include "cpu.h"
#include "cpudisasm.h"

int gbemu_disasm_current(gbemu_cpu_t* CPU)
{
   static const char* reg_names[] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
   static const char* reg16_names[] = {"BC", "DE", "HL", "SP"};
   static const char* reg16_addr_names[] = {"(BC)", "(DE)", "(HL+)", "(HL-)"};
   static const char* cond_names[] = {"NZ", "Z", "NC", "C"};
   static const char* RST_labels[] = {"00H", "08H", "10H", "18H","20H", "28H", "30H", "38H"};
   static const char* num_labels[] = {"0", "1", "2", "3","4", "5", "6", "7"};

   char immediate8[3];
   char sp_plus_immediate8[6];
   char immediate16[5];
   char immediate8_addr[5];
   char immediate16_addr[7];

   struct
   {
      union
      {
         struct
         {
            unsigned r1        : 3;
            unsigned r0        : 3;
            unsigned m11000000 : 2;
         };
         struct
         {
            unsigned m00000111   : 3;
            unsigned m00001000   : 1;
            unsigned r2          : 2;
            unsigned             : 2;

         };
         struct
         {
            unsigned             : 3;
            unsigned r3          : 2;
            unsigned m00100000   : 1;
            unsigned             : 2;

         };
         uint8_t val;
      };
      const char* label;
      const char* operand0;
      const char* operand1;
      char* zero;
      char* negative;
      char* halfcarry;
      char* carry;
      int cycles;
      int cycles2;
      int size;

   } op;

   op.val         = GB.MEMORY[CPU->PC];
   op.label       = NULL;
   op.operand0    = NULL;
   op.operand1    = NULL;
   op.zero        = "-";
   op.negative    = "-";
   op.halfcarry   = "-";
   op.carry       = "-";
   op.cycles      = 1;
   op.cycles2     = 0;
   op.size        = 1;


   snprintf(immediate8, sizeof(immediate8), "%02X", GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(sp_plus_immediate8, sizeof(sp_plus_immediate8), "SP+%02X", GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(immediate16, sizeof(immediate16), "%02X%02X", GB.MEMORY[(CPU->PC + 2) & 0xFFFF],
            GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(immediate8_addr, sizeof(immediate8_addr), "(%02X)", GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   snprintf(immediate16_addr, sizeof(immediate16_addr), "(%02X%02X)", GB.MEMORY[(CPU->PC + 2) & 0xFFFF],
            GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);

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
   case 0x07:
      op.label = "RLCA";
      op.zero = "0";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "C";
      break;
   case 0x0F:
      op.label = "RRCA";
      op.zero = "0";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "C";
      break;
   case 0x17:
      op.label = "RLA";
      op.zero = "0";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "C";
      break;
   case 0x1F:
      op.label = "RRA";
      op.zero = "0";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "C";
      break;
   case 0x27:
      op.label = "DAA";
      op.zero = "Z";
      op.halfcarry = "0";
      op.carry = "C";
      break;
   case 0x2F:
      op.label = "CPL";
      op.negative = "1";
      op.halfcarry = "1";
      break;
   case 0x37:
      op.label = "SCF";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "1";
      break;
   case 0x3F:
      op.label = "CCF";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "C";
      break;
   case 0x76:
      op.label = "HALT";
      break;
   case 0xC3:
      op.label = "JP";
      op.operand0 = immediate16;
      op.size += 2;
      op.cycles += 3;
      break;
   case 0xF3:
      op.label = "DI";
      break;
   case 0xFB:
      op.label = "EI";
      break;
   case 0xCB:
      op.val = GB.MEMORY[(CPU->PC + 1) & 0xFFFF];
      op.size++;
      op.cycles++;
      if (op.r1 == 0b110)
         op.cycles += 2;
      switch (op.m11000000)
      {
      op.operand1 = reg_names[op.r1];
      op.zero = "Z";
      op.negative = "0";
      op.halfcarry = "0";
      op.carry = "C";
      case 0b00:
         switch (op.r0)
         {
         case 0b000:
            op.label = "RLC";
            break;
         case 0b001:
            op.label = "RRC";
            break;
         case 0b010:
            op.label = "RL";
            break;
         case 0b011:
            op.label = "RR";
            break;
         case 0b100:
            op.label = "SLA";
            break;
         case 0b101:
            op.label = "SRA";
            op.carry = "0";
            break;
         case 0b110:
            op.label = "SWAP";
            op.carry = "0";
            break;
         case 0b111:
            op.label = "SRL";
            break;
         }
         break;
      case 0b01:
         op.label = "BIT";
         op.operand0 = num_labels[op.r0];
         op.zero = "Z";
         op.negative = "0";
         op.halfcarry = "1";
         break;
      case 0b10:
         op.label = "RES";
         op.operand0 = num_labels[op.r0];
         break;
      case 0b11:
         op.label = "SET";
         op.operand0 = num_labels[op.r0];
         break;
      }
      break;
   case 0xCD:
      op.label = "CALL";
      op.operand0 = immediate16;
      op.size += 2;
      op.cycles += 5;
      break;
   case 0xE0:
      op.label = "LDH";
      op.operand0 = immediate8_addr;
      op.size++;
      op.operand1 = "A";
      op.cycles = 3;
      break;
   case 0xF0:
      op.label = "LDH";
      op.operand0 = "A";
      op.operand1 = immediate8_addr;
      op.size++;
      op.cycles = 3;
      break;
   case 0xE8:
      op.label = "ADD";
      op.operand0 = "SP";
      op.operand1 = immediate8;
      op.size++;
      op.cycles = 4;
      op.zero = "0";
      op.negative = "0";
      op.halfcarry = "H";
      op.carry = "C";
      break;
   case 0xF8:
      op.label = "LD";
      op.operand0 = "HL";
      op.operand1 = sp_plus_immediate8;
      op.size++;
      op.cycles = 3;
      op.zero = "0";
      op.negative = "0";
      op.halfcarry = "H";
      op.carry = "C";
      break;
   case 0xD3:
   case 0xDB:
   case 0xDD:
   case 0xE3:
   case 0xE4:
   case 0xEB:
   case 0xEC:
   case 0xED:
   case 0xF4:
   case 0xFC:
   case 0xFD:
      op.label = "INVALID";
   default:
      switch (op.m11000000)
      {
      case 0b00:
         switch (op.m00000111)
         {
         case 0b000:
            op.label = "JR";
            op.size++;
            op.cycles += 2;
            if (op.m00100000 == 0)
               op.operand0 = immediate8;
            else
            {
               op.operand0 = cond_names[op.r3];
               op.cycles2 = 2;
               op.operand1 = immediate8;
            }
            break;
         case 0b001:
            if (op.m00001000 == 0)
            {
               op.label = "LD";
               op.operand0 = reg16_names[op.r2];
               op.operand1 = immediate16;
               op.size += 2;
               op.cycles += 2;
            }
            else
            {
               op.label = "ADD";
               op.operand0 = "HL";
               op.operand1 = reg16_names[op.r2];
               op.cycles ++;
               op.negative = "0";
               op.halfcarry = "H";
               op.carry = "C";
            }
            break;
         case 0b010:
            op.label = "LD";
            op.cycles++;
            if (op.m00001000 == 0)
            {
               op.operand0 = reg16_addr_names[op.r2];
               op.operand1 = "A";
            }
            else
            {
               op.operand0 = "A";
               op.operand1 = reg16_addr_names[op.r2];
            }
            break;
         case 0b011:
            op.operand0 = reg16_names[op.r2];
            op.cycles++;
            if (op.m00001000 == 0)
               op.label = "INC";
            else
               op.label = "DEC";
            break;
         case 0b100:
            op.label = "INC";
            op.operand0 = reg_names[op.r0];
            op.zero = "Z";
            op.negative = "0";
            op.halfcarry = "H";
            if (op.r0 == 0b110)
               op.cycles += 2;
            break;
         case 0b101:
            op.label = "DEC";
            op.operand0 = reg_names[op.r0];
            op.zero = "Z";
            op.negative = "1";
            op.halfcarry = "H";
            if (op.r0 == 0b110)
               op.cycles += 2;
            break;
         case 0b110:
            op.label = "LD";
            op.operand0 = reg_names[op.r0];
            op.operand1 = immediate8;
            op.size++;
            op.cycles ++;
            if (op.r0 == 0b110)
               op.cycles ++;
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
         op.zero = "Z";
         op.operand0 = "A";
         op.operand1 = reg_names[op.r1];
         if (op.r0 == 0b110)
            op.cycles++;
         switch (op.r0)
         {
         case 0b000:
            op.label = "ADD";
            op.negative = "0";
            op.halfcarry = "H";
            op.carry = "C";
            break;
         case 0b001:
            op.label = "ADC";
            op.negative = "0";
            op.halfcarry = "H";
            op.carry = "C";
            break;
         case 0b010:
            op.label = "SUB";
            op.negative = "1";
            op.halfcarry = "H";
            op.carry = "C";
            break;
         case 0b011:
            op.label = "SBC";
            op.negative = "1";
            op.halfcarry = "H";
            op.carry = "C";
            break;
         case 0b100:
            op.label = "AND";
            op.negative = "0";
            op.halfcarry = "1";
            op.carry = "0";
            break;
         case 0b101:
            op.label = "XOR";
            op.negative = "0";
            op.halfcarry = "0";
            op.carry = "0";
            break;
         case 0b110:
            op.label = "OR";
            op.negative = "0";
            op.halfcarry = "0";
            op.carry = "0";
            break;
         case 0b111:
            op.label = "CP";
            op.negative = "1";
            op.halfcarry = "H";
            op.carry = "C";
            break;
         }
         break;
      case 0b11:
         switch (op.m00000111)
         {
         case 0b000:
            op.label = "RET";
            op.cycles = 5;
            op.cycles2 = 2;
            op.operand0 = cond_names[op.r3];
            break;
         case 0b001:
            op.label = "POP";
            op.cycles = 3;
            if (op.r2 == 0b11)
            {
               op.operand0 = "AF";
               op.zero = "Z";
               op.negative = "N";
               op.halfcarry = "H";
               op.carry = "C";
            }
            else
               op.operand0 = reg16_names[op.r2];
            break;
         case 0b010:
            if (op.m00100000 == 0)
            {
               op.label = "JP";
               op.operand0 = cond_names[op.r3];
               op.operand1 = immediate16;
               op.size += 2;
               op.cycles = 4;
               op.cycles2 = 3;
            }
            else
            {
               op.label = "LD";
               switch (op.r3)
               {
               case 0b00:
                  op.operand0 = "(FF00 + C)";
                  op.operand1 = "A";
                  op.size++;
                  break;
               case 0b10:
                  op.operand0 = "A";
                  op.operand1 = "(FF00 + C)";
                  op.size++;
                  break;
               case 0b01:
                  op.operand0 = immediate16_addr;
                  op.operand1 = "A";
                  op.size += 2;
                  op.cycles = 4;
                  break;
               case 0b11:
                  op.operand0 = "A";
                  op.operand1 = immediate16_addr;
                  op.size += 2;
                  op.cycles = 4;
                  break;
               }
            }
            break;
         // case 0b011:
         //    break;
         case 0b100:
            op.label = "CALL";
            op.operand0 = cond_names[op.r3];
            op.operand1 = immediate16;
            op.size += 2;
            op.cycles = 6;
            op.cycles2 = 3;
            break;
         case 0b101:
            op.label = "PUSH";
            op.cycles = 4;
            if (op.r2 == 0b11)
               op.operand0 = "AF";
            else
               op.operand0 = reg16_names[op.r2];
            break;
         case 0b110:
            op.operand0 = "A";
            op.operand1 = immediate8;
            op.size++;
            op.cycles++;
            op.zero = "Z";
            switch (op.r0)
            {
            case 0b000:
               op.label = "ADD";
               op.negative = "0";
               op.halfcarry = "H";
               op.carry = "C";
               break;
            case 0b001:
               op.label = "ADC";
               op.negative = "0";
               op.halfcarry = "H";
               op.carry = "C";
               break;
            case 0b010:
               op.label = "SUB";
               op.negative = "1";
               op.halfcarry = "H";
               op.carry = "C";
               break;
            case 0b011:
               op.label = "SBC";
               op.negative = "1";
               op.halfcarry = "H";
               op.carry = "C";
               break;
            case 0b100:
               op.label = "AND";
               op.negative = "0";
               op.halfcarry = "1";
               op.carry = "0";
               break;
            case 0b101:
               op.label = "XOR";
               op.negative = "0";
               op.halfcarry = "0";
               op.carry = "0";
               break;
            case 0b110:
               op.label = "OR";
               op.negative = "0";
               op.halfcarry = "0";
               op.carry = "0";
               break;
            case 0b111:
               op.label = "CP";
               op.negative = "1";
               op.halfcarry = "H";
               op.carry = "C";
               break;
            }
            break;
         case 0b111:
            op.label = "RST";
            op.cycles = 4;
            op.operand0 = RST_labels[op.r0];
            break;
         }
         break;
      }
      break;
   }

   printf("(%04X) ", CPU->PC);

   if (op.size == 1)
      printf("%02X       :", GB.MEMORY[CPU->PC]);
   else if (op.size == 2)
      printf("%02X %02X    :", GB.MEMORY[CPU->PC], GB.MEMORY[(CPU->PC + 1) & 0xFFFF]);
   else
      printf("%02X %02X %02X :", GB.MEMORY[CPU->PC], GB.MEMORY[(CPU->PC + 1) & 0xFFFF], GB.MEMORY[(CPU->PC + 2) & 0xFFFF]);

   if (op.label)
   {
      printf(" %i", op.cycles);

      if (op.cycles2)
         printf("|%i", op.cycles2);
      else
         printf("  ");

      printf(" (%s %s %s %s)  %s", op.zero, op.negative, op.halfcarry, op.carry, op.label);

      if (op.operand0)
      {
         printf(" %s", op.operand0);
         if (op.operand1)
            printf(", %s", op.operand1);
      }
      else
         if (op.operand1)
            printf("%s", op.operand1);

   }
   else
      printf(" ?   (? ? ? ?)  ????");




   printf("\n");
   fflush(stdout);
   return op.size;
}

void gbemu_dump_state(gbemu_cpu_t* CPU)
{

   printf("%8u PC: %04X SP: %04X AF: %04X BC: %04X DE: %04X HL: %04X (Z:%u N:%u H:%u C:%u)\n",
          CPU->cycles, CPU->PC, CPU->SP, CPU->AF, CPU->BC, CPU->DE, CPU->HL,
          CPU->FZ, CPU->FN, CPU->FH, CPU->FC);
   fflush(stdout);
}
