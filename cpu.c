#include "gbemu.h"
#include <stdint.h>
#include <signal.h>


void gbemu_cpu_run(void)
{
   gbemu_cpu_t CPU = GB.CPU;

   switch (GB.ROM[CPU.PC++])
   {
   case 0x00:
      //
      break;
   default:
      printf("unknown opcode : 0x%02X\n", GB.ROM[CPU.PC - 1]);
      fflush(stdout);
      raise(SIGINT);
      break;
   }

   GB.CPU = CPU;
}
