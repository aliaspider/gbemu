#include "gbemu.h"
#include "cpudisasm.h"
#include <string.h>

gbemu_state_t GB;

bool gbemu_load_game(const void* data, size_t size, const void* bios_data)
{
   int i;

   cartridge_info_t* cart_info = gbemu_get_cart_info(GB.HEADER.cart_info_id);

   if(size > sizeof(GB.ROM))
      size = sizeof(GB.ROM);
   memcpy(GB.ROM, data, size);

   if(bios_data)
   {
      memcpy(GB.BIOS, bios_data, 0x100);
      printf("bios : ");
      for(i = 0; i< 0x100; i++)
      {
         if(!(i&0xF))
            printf("\n");
         printf("%02X ",GB.BIOS[i]);
      }
      printf("\n");
   }

   printf("header info\n");
   printf("buffer0 : ");
   for(i = 0; i< 0x100; i++)
   {
      if(!(i&0xF))
         printf("\n");
      printf("%02X ",GB.HEADER.buffer0[i]);
   }
   printf("\n");
   printf("startup : 0x%08X\n", *(uint32_t*)GB.HEADER.startup);
   printf("logo : ");
   for(i = 0; i< 0x30; i++)
   {
      if(!(i&0xF))
         printf("\n");
      printf("%02X ",GB.HEADER.logo[i]);
   }
   printf("\n");

   printf("Title : %s\n", GB.HEADER.title);
   printf("gbc Title : %s\n", GB.HEADER.gbc_title);
   printf("gbc manufacturer code : 0x%02X%02X%02X%02X\n",
          (uint32_t)GB.HEADER.gbc_manufacturer_code[0],
          (uint32_t)GB.HEADER.gbc_manufacturer_code[1],
          (uint32_t)GB.HEADER.gbc_manufacturer_code[2],
          (uint32_t)GB.HEADER.gbc_manufacturer_code[3]);
   printf("gbc flag : 0x%02X\n", (uint32_t)GB.HEADER.gbc_flag);

   printf("new licencee code : 0x%04X\n", *(uint16_t*)GB.HEADER.new_licencee_code);
   printf("super gameboy flag : 0x%02X\n", (uint32_t)GB.HEADER.sgb_flag);

   printf("cart type : 0x%02X --> %s%s%s%s%s\n",(uint32_t)GB.HEADER.cart_info_id,
          gbemu_get_cart_type_ident(cart_info->type), cart_info->RAM?"+RAM":"",
          cart_info->BATTERY?"+BATTERY":"", cart_info->TIMER?"+TIMER":"",
          cart_info->RUMBLE?"+RUMBLE":"");
   printf("cart type : 0x%02X\n", (uint32_t)GB.HEADER.cart_info_id);
   printf("rom size : 0x%X --> 0x%X(%u, %u banks)\n", GB.HEADER.rom_size_id,
          gbemu_get_rom_size(GB.HEADER.rom_size_id),
          gbemu_get_rom_size(GB.HEADER.rom_size_id),
          gbemu_get_rom_size(GB.HEADER.rom_size_id) >> 14);
   printf("ram size : %u\n", gbemu_get_ram_size(GB.HEADER.ram_size_id));
   printf("destination code : 0x%02X (%sJapanese)\n", (uint32_t)GB.HEADER.destination_code, GB.HEADER.destination_code?"non-":"");
   printf("old licencee code : 0x%02X\n", (uint32_t)GB.HEADER.old_licencee_code);
   printf("version number : 0x%02X\n", (uint32_t)GB.HEADER.version_number);
   printf("header checksum : 0x%02X\n", (uint32_t)GB.HEADER.header_checksum);
   printf("global checksum : 0x%02X%02X\n", (uint32_t)GB.HEADER.global_checksum_high
          , (uint32_t)GB.HEADER.global_checksum_low);



   fflush(stdout);
//   exit(0);

   GB.CPU.AF = 0;
   GB.CPU.BC = 0;
   GB.CPU.DE = 0;
   GB.CPU.HL = 0;
   GB.CPU.SP = 0xFFFE;
   GB.CPU.PC = 0x100;
   GB.CPU.cycles = 0;
   GB.CPU.interrupts_enabled = 1;
}

#if 1
void gbemu_run(void)
{
   gbemu_cpu_run(0x8000);
}
#else
void gbemu_run(void)
{
//   GB.CPU.PC = 0x150;
//   while (GB.CPU.PC < 0x300)
//      GB.CPU.PC += gbemu_disasm_current(&GB.CPU);
   int i;
   for (i = 0; i< 0x20; i++)
      GB.CPU.PC += gbemu_disasm_current(&GB.CPU);
   DEBUG_HOLD();
}
#endif