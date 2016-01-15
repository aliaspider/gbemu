
#include "gbemu.h"
#include "libretro.h"
#include "cpudisasm.h"

#include <string.h>
#include <stdarg.h>

gbemu_state_t GB;
static bool has_bios;

#define gb_check_addr(reg, addr, size) \
   do {\
      if(((uint8_t*)&reg != &GB.MEMORY[addr]) || (sizeof(reg) != size)) \
      {\
         printf("reg : %s\n expected: addr = 0x%04X size = 0x%X\n got:      addr = 0x%04X size = 0x%X\n ", #reg, \
             addr, size, (unsigned)((uint8_t*)&reg - GB.MEMORY), (unsigned)sizeof(reg));\
         exit(1);\
      }\
   }while(0)

#define gb_check_register8(reg, addr) gb_check_addr(reg, addr, 1)


void gbemu_printf(const char* fmt, ...)
{
   extern retro_log_printf_t log_cb;
   if(log_cb)
   {
      va_list ap;

      va_start(ap, fmt);
      vprintf(fmt, ap);
      va_end(ap);
   }
}

void gbemu_sanity_checks(void)
{
   gb_check_addr(GB.ROM, 0x0, 0x8000);
   gb_check_addr(GB.ROM00, 0x0, 0x4000);
   gb_check_addr(GB.ROMXX, 0x4000, 0x4000);
   gb_check_addr(GB.HEADER, 0x0, 0x150);
   gb_check_addr(GB.VRAM, 0x8000, 0x2000);
   gb_check_addr(GB.WRAM, 0xC000, 0x2000);
   gb_check_addr(GB.WRAM0, 0xC000, 0x1000);
   gb_check_addr(GB.WRAM1, 0xD000, 0x1000);
   gb_check_addr(GB.ECHO, 0xE000, 0x1E00);
   gb_check_addr(GB.OAM, 0xFE00, 0xA0);
   gb_check_addr(GB.unused, 0xFEA0, 0x60);
   gb_check_addr(GB.IO, 0xFF00, 0x80);
   gb_check_addr(GB.SND_regs.WAVE_TABLE, 0xFF30, 0x10);
   gb_check_addr(GB.SND_regs.channels.square1, 0xFF10, 0x5);
   gb_check_addr(GB.SND_regs.channels.square2, 0xFF15, 0x5);
   gb_check_addr(GB.SND_regs.channels.wave, 0xFF1A, 0x5);
   gb_check_addr(GB.SND_regs.channels.noise, 0xFF1F, 0x5);
   gb_check_addr(GB.SND_regs.channels.master, 0xFF24, 0x3);
   gb_check_addr(GB.HRAM, 0xFF80, 0x7F);

   gb_check_register8(GB.DIV, 0xFF04);
   gb_check_register8(GB.TIMA, 0xFF05);
   gb_check_register8(GB.TMA, 0xFF06);
   gb_check_register8(GB.TAC, 0xFF07);

   gb_check_register8(GB.IF, 0xFF0F);
   gb_check_register8(GB.SND_regs.NR10, 0xFF10);
   gb_check_register8(GB.SND_regs.NR11, 0xFF11);
   gb_check_register8(GB.SND_regs.NR12, 0xFF12);
   gb_check_register8(GB.SND_regs.NR13, 0xFF13);
   gb_check_register8(GB.SND_regs.NR14, 0xFF14);
   gb_check_register8(GB.SND_regs.NR21, 0xFF16);
   gb_check_register8(GB.SND_regs.NR22, 0xFF17);
   gb_check_register8(GB.SND_regs.NR23, 0xFF18);
   gb_check_register8(GB.SND_regs.NR24, 0xFF19);
   gb_check_register8(GB.SND_regs.NR30, 0xFF1A);
   gb_check_register8(GB.SND_regs.NR31, 0xFF1B);
   gb_check_register8(GB.SND_regs.NR32, 0xFF1C);
   gb_check_register8(GB.SND_regs.NR33, 0xFF1D);
   gb_check_register8(GB.SND_regs.NR34, 0xFF1E);
   gb_check_register8(GB.SND_regs.NR41, 0xFF20);
   gb_check_register8(GB.SND_regs.NR42, 0xFF21);
   gb_check_register8(GB.SND_regs.NR43, 0xFF22);
   gb_check_register8(GB.SND_regs.NR44, 0xFF23);
   gb_check_register8(GB.SND_regs.NR50, 0xFF24);
   gb_check_register8(GB.SND_regs.NR51, 0xFF25);
   gb_check_register8(GB.SND_regs.NR52, 0xFF26);



   gb_check_register8(GB.LCDC, 0xFF40);
   gb_check_register8(GB.LCD_STAT, 0xFF41);
   gb_check_register8(GB.SCY, 0xFF42);
   gb_check_register8(GB.SCX, 0xFF43);
   gb_check_register8(GB.LY, 0xFF44);
   gb_check_register8(GB.LYC, 0xFF45);
   gb_check_register8(GB.DMA, 0xFF46);
   gb_check_register8(GB.BGP, 0xFF47);
   gb_check_register8(GB.OBP0, 0xFF48);
   gb_check_register8(GB.OBP1, 0xFF49);
   gb_check_register8(GB.WY, 0xFF4A);
   gb_check_register8(GB.WX, 0xFF4B);
   gb_check_register8(GB.IE, 0xFFFF);

}


void gbemu_reset(void)
{
   memset(&GB.APU, 0x00, sizeof(GB.APU));
   memset(&GB.CPU, 0x00, sizeof(GB.CPU));
   memset(&GB.VRAM, 0x00, sizeof(GB.VRAM));
   memset(&GB.OAM, 0x00, sizeof(GB.OAM));
   memset(&GB.WRAM, 0x00, sizeof(GB.HRAM));
   memset(&GB.IO, 0x00, sizeof(GB.IO));
   memset(&GB.MEMORY[0xFF00], 0x00, 0x100);
   memset(&GB.serial_port, 0x00, sizeof(GB.serial_port));
#if 1
   GB.CPU.AF = 0x01B0;
   GB.CPU.BC = 0x0013;
   GB.CPU.DE = 0x00D8;
   GB.CPU.HL = 0x014D;
   GB.CPU.SP = 0xFFFE;
#ifdef USE_BIOS
   if(has_bios)
      GB.CPU.PC = 0x0000;
   else
#endif
   GB.CPU.PC = 0x0100;
   GB.CPU.cycles = 0;
   GB.CPU.IME = 1;
   GB.CPU.HALT = 0;

   GB.MEMORY[0xFF00] = 0x0F; //clear input
   GB.MEMORY[0xFF05] = 0x00; // TIMA
   GB.MEMORY[0xFF06] = 0x00; // TMA
   GB.MEMORY[0xFF07] = 0x00; // TAC
   GB.MEMORY[0xFF10] = 0x80; // NR10
   GB.MEMORY[0xFF11] = 0xBF; // NR11
   GB.MEMORY[0xFF12] = 0xF3; // NR12
   GB.MEMORY[0xFF14] = 0xBF; // NR14
   GB.MEMORY[0xFF16] = 0x3F; // NR21
   GB.MEMORY[0xFF17] = 0x00; // NR22
   GB.MEMORY[0xFF19] = 0xBF; // NR24
   GB.MEMORY[0xFF1A] = 0x7F; // NR30
   GB.MEMORY[0xFF1B] = 0xFF; // NR31
   GB.MEMORY[0xFF1C] = 0x9F; // NR32
   GB.MEMORY[0xFF1E] = 0xBF; // NR33
   GB.MEMORY[0xFF20] = 0xFF; // NR41
   GB.MEMORY[0xFF21] = 0x00; // NR42
   GB.MEMORY[0xFF22] = 0x00; // NR43
   GB.MEMORY[0xFF23] = 0xBF; // NR30
   GB.MEMORY[0xFF24] = 0x77; // NR50
   GB.MEMORY[0xFF25] = 0xF3; // NR51
   GB.MEMORY[0xFF26] = 0xF1; // NR52 (GB = 0xF1, SGB = 0xF0)
   GB.MEMORY[0xFF40] = 0x91; // LCDC
   GB.MEMORY[0xFF42] = 0x00; // SCY
   GB.MEMORY[0xFF43] = 0x00; // SCX
   GB.MEMORY[0xFF45] = 0x00; // LYC
   GB.MEMORY[0xFF47] = 0xFC; // BGP
   GB.MEMORY[0xFF48] = 0xFF; // OBP0
   GB.MEMORY[0xFF49] = 0xFF; // OBP1
   GB.MEMORY[0xFF4A] = 0x00; // WY
   GB.MEMORY[0xFF4B] = 0x00; // WX
   GB.MEMORY[0xFFFF] = 0x00; // IE
//   memset(GB.HRAM, 0, sizeof(GB.HRAM));
//   DEBUG_HOLD();
#else
   GB.CPU.AF = 0;
   GB.CPU.BC = 0;
   GB.CPU.DE = 0;
   GB.CPU.HL = 0;
   GB.CPU.SP = 0xFFFE;
   GB.CPU.PC = 0x100;
   GB.CPU.cycles = 0;
   GB.CPU.interrupts_enabled = 1;
#endif


   GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[0][1];
   GB.MBC.active_SRAM_bank = GB.MBC.SRAM_banks[0];
   GB.MBC.SRAM_enable = false;
   GB.MBC.SRAM_banking_mode = false;
   GB.MBC.bank_id_low = 1;
   GB.MBC.bank_id_high = 0;

   if (GB.cart_info->type == CART_TYPE_MBC5)
      GB.MBC.type = CART_TYPE_MBC1;
   else
      GB.MBC.type = GB.cart_info->type;

}

bool gbemu_load_game(const void* data, size_t size, const void* bios_data)
{
   int i;

   gbemu_sanity_checks();

   if(size > sizeof(GB.MBC.ROM))
      size = sizeof(GB.MBC.ROM);
   memcpy(GB.MBC.ROM, data, size);

   if(size > sizeof(GB.ROM))
      size = sizeof(GB.ROM);
   memcpy(GB.ROM, data, size);

   GB.cart_info = gbemu_get_cart_info(GB.HEADER.cart_info_id);

#ifdef USE_BIOS
   memcpy(GB.BIOS, GB.MEMORY, 0x100);
#endif


   if(bios_data)
   {
#ifdef USE_BIOS
      has_bios = true;
      memcpy(GB.MEMORY, bios_data, 0x100);
#else
      memcpy(GB.BIOS, bios_data, 0x100);
#endif
      gbemu_printf("bios : ");
      for(i = 0; i< 0x100; i++)
      {
         if(!(i&0xF))
            gbemu_printf("\n");
         gbemu_printf("%02X ",GB.BIOS[i]);
      }
      gbemu_printf("\n");
   }
   else
      has_bios = false;

   gbemu_printf("header info\n");
   gbemu_printf("buffer0 : ");
   for(i = 0; i< 0x100; i++)
   {
      if(!(i&0xF))
         gbemu_printf("\n");
      gbemu_printf("%02X ",GB.HEADER.buffer0[i]);
   }
   gbemu_printf("\n");
   gbemu_printf("startup : 0x%08X\n", *(uint32_t*)GB.HEADER.startup);
   gbemu_printf("logo : ");
   for(i = 0; i< 0x30; i++)
   {
      if(!(i&0xF))
         gbemu_printf("\n");
      gbemu_printf("%02X ",GB.HEADER.logo[i]);
   }
   gbemu_printf("\n");

   gbemu_printf("Title : %s\n", GB.HEADER.title);
   gbemu_printf("gbc Title : %s\n", GB.HEADER.gbc_title);
   gbemu_printf("gbc manufacturer code : 0x%02X%02X%02X%02X\n",
          (uint32_t)GB.HEADER.gbc_manufacturer_code[0],
          (uint32_t)GB.HEADER.gbc_manufacturer_code[1],
          (uint32_t)GB.HEADER.gbc_manufacturer_code[2],
          (uint32_t)GB.HEADER.gbc_manufacturer_code[3]);
   gbemu_printf("gbc flag : 0x%02X\n", (uint32_t)GB.HEADER.gbc_flag);

   gbemu_printf("new licencee code : 0x%04X\n", *(uint16_t*)GB.HEADER.new_licencee_code);
   gbemu_printf("super gameboy flag : 0x%02X\n", (uint32_t)GB.HEADER.sgb_flag);

   gbemu_printf("cart type : 0x%02X --> %s%s%s%s%s\n",(uint32_t)GB.HEADER.cart_info_id,
          gbemu_get_cart_type_ident(GB.cart_info->type), GB.cart_info->RAM?"+RAM":"",
          GB.cart_info->BATTERY?"+BATTERY":"", GB.cart_info->TIMER?"+TIMER":"",
          GB.cart_info->RUMBLE?"+RUMBLE":"");
   gbemu_printf("cart type : 0x%02X\n", (uint32_t)GB.HEADER.cart_info_id);
   gbemu_printf("rom size : 0x%X --> 0x%X(%u, %u banks)\n", GB.HEADER.rom_size_id,
          gbemu_get_rom_size(GB.HEADER.rom_size_id),
          gbemu_get_rom_size(GB.HEADER.rom_size_id),
          gbemu_get_rom_size(GB.HEADER.rom_size_id) >> 14);
   gbemu_printf("ram size : %u\n", gbemu_get_ram_size(GB.HEADER.ram_size_id));
   gbemu_printf("destination code : 0x%02X (%sJapanese)\n", (uint32_t)GB.HEADER.destination_code, GB.HEADER.destination_code?"non-":"");
   gbemu_printf("old licencee code : 0x%02X\n", (uint32_t)GB.HEADER.old_licencee_code);
   gbemu_printf("version number : 0x%02X\n", (uint32_t)GB.HEADER.version_number);
   gbemu_printf("header checksum : 0x%02X\n", (uint32_t)GB.HEADER.header_checksum);
   gbemu_printf("global checksum : 0x%02X%02X\n", (uint32_t)GB.HEADER.global_checksum_high
          , (uint32_t)GB.HEADER.global_checksum_low);

//   gbemu_wait_for_input();

   fflush(stdout);
//   exit(0);

   gbemu_reset();

   return true;
}

#if 1
void gbemu_run(void)
{
//   DEBUG_HOLD();
   gbemu_cpu_run(0x8000);
}
#else
void gbemu_run(void)
{
//   GB.CPU.PC = 0x150;
//   while (GB.CPU.PC < 0x300)
//      GB.CPU.PC += gbemu_disasm_current(&GB.CPU, false);
   int i;
   for (i = 0; i< 0x20; i++)
      GB.CPU.PC += gbemu_disasm_current(&GB.CPU);
   DEBUG_HOLD();
}
#endif
