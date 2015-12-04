#ifndef GBEMU_H
#define GBEMU_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "retro_miscellaneous.h"

#include "cpu.h"
#include "cart.h"


typedef struct
{
   union
   {
      struct
      {
         union
         {
            struct
            {
               uint8_t ROM00[0x4000];
               uint8_t ROMXX[0x4000];
            };
            uint8_t ROM[0x8000];
            gbemu_rom_header_t HEADER;
         };
         uint8_t VRAM[0x2000];
         union
         {
            struct
            {
               uint8_t WRAM0[0x1000];
               uint8_t WRAM1[0x1000];
            };
            uint8_t WRAM[0x2000];
         };
         uint8_t ECHO[0x1E00];
         uint8_t OAM[0xA0];
         uint8_t unused[0x60];
         uint8_t IO[0x80];
         uint8_t HRAM[0x7F];
         uint8_t IE_reg;
      };
      uint8_t MEMORY [0x10000];
      int8_t  sMEMORY[0x10000];
   };
   uint8_t BIOS[0x100];
   gbemu_cpu_t CPU;
}gbemu_state_t;

extern gbemu_state_t GB;

void gbemu_run(void);

bool gbemu_load_game(const void* data, size_t size, const void* bios_data);


#define DEBUG_HOLD() do{printf("%s@%s:%d.\n",__FUNCTION__, __FILE__, __LINE__);fflush(stdout);gbemu_wait_for_input();}while(0)
void gbemu_wait_for_input(void);
void gbemu_check_exit_request(void);

#endif // GBEMU_H
