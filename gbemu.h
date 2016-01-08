#ifndef GBEMU_H
#define GBEMU_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "retro_miscellaneous.h"

#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"

#define GB_LINE_TICK_COUNT  114
#define GB_V_COUNT   154
#define GB_FRAME_TICK_COUNT  (GB_LINE_TICK_COUNT * GB_V_COUNT)

#define GB_LCD_STAT_MODE0_HBLANK          0
#define GB_LCD_STAT_MODE1_VBLANK          1
#define GB_LCD_STAT_MODE2_OAM_busy        2
#define GB_LCD_STAT_MODE3_OAM_VRAM_busy   3

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
         uint8_t cart_RAM[0x2000];
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
         union
         {
            uint8_t IO[0x80];  // @0xFF00
            struct __attribute__((packed))
            {
               uint8_t JOYP;
               uint8_t SB; // serial data
               uint8_t SC; // serial control
               unsigned : 8;
               uint8_t DIV; // Divider Register
               uint8_t TIMA; // Timer Counter;
               uint8_t TMA; // Timer Modulo;
               struct __attribute__((packed))
               {
                  unsigned clock_select : 2;
                  unsigned active : 1;
               }
               TAC; // Timer Control;

               uint8_t IO_unused0[0x7];

               struct __attribute__((packed))
               {
                  unsigned Vblank   : 1;
                  unsigned LCD_stat : 1;
                  unsigned timer    : 1;
                  unsigned serial   : 1;
                  unsigned joypad   : 1;
               }
               IF;
               gbemu_sound_regs_t SND_regs;
               uint8_t LCDC;
               struct __attribute__((packed))
               {
                  unsigned mode_flag      : 2;
                  unsigned LCY_eq_LY_flag : 1;
                  unsigned HBlank_IE      : 1;
                  unsigned VBlank_IE      : 1;
                  unsigned OAM_IE         : 1;
                  unsigned LCY_eq_LY_IE   : 1;
               }
               LCD_STAT;
            };
         };

         uint8_t HRAM[0x7F];
         struct __attribute__((packed))
         {
            unsigned Vblank   : 1;
            unsigned LCD_stat : 1;
            unsigned timer    : 1;
            unsigned serial   : 1;
            unsigned joypad   : 1;
         } IE;
      };
      uint8_t MEMORY [0x10000];
      int8_t  sMEMORY[0x10000];      
   };
   uint8_t BIOS[0x100];
   const cartridge_info_t* cart_info;
   gbemu_cpu_t CPU;
   gbemu_apu_t APU;
   struct
   {
      bool SRAM_banking_mode;
      bool SRAM_enable;
      uint8_t bank_id_low;
      uint8_t bank_id_high;
      union
      {
         uint8_t ROM_banks[0x4][0x20][0x4000];
         uint8_t ROM[0x200000];
      };
      union
      {
         uint8_t SRAM_banks[0x4][0x2000];
         uint8_t SRAM[0x8000];
      };
      uint8_t* active_ROM_bank;
      uint8_t* active_SRAM_bank;

      catridge_type_enum type; //duplicate

   }MBC;

} gbemu_state_t;

extern gbemu_state_t GB;

void gbemu_run(void);

bool gbemu_load_game(const void* data, size_t size, const void* bios_data);


#define DEBUG_HOLD() do{printf("%s@%s:%d.\n",__FUNCTION__, __FILE__, __LINE__);fflush(stdout);gbemu_wait_for_input();}while(0)
void gbemu_wait_for_input(void);
void gbemu_check_exit_request(void);

#ifndef NDEBUG
#ifdef _WIN32
#define DEBUG_BREAK() DebugBreak()
#else
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#endif
#else
#define DEBUG_BREAK()
#endif

#endif // GBEMU_H
