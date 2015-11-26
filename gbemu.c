#include "gbemu.h"
#include <string.h>

typedef enum
{
   CART_TYPE_INVALID,
   CART_TYPE_GENERIC,
   CART_TYPE_MBC1,
   CART_TYPE_MBC2,
   CART_TYPE_MBC3,
   CART_TYPE_MBC4,
   CART_TYPE_MBC5,
   CART_TYPE_MMM01,
   CART_TYPE_POCKET_CAMERA,
   CART_TYPE_BANDAI_TAMA5,
   CART_TYPE_HuC3,
   CART_TYPE_HuC1
} catridge_type_enum;

const char* cartridge_type_str[] =
{
   "invalid",
   "ROM",
   "MBC1",
   "MBC2",
   "MBC3",
   "MBC4",
   "MBC5",
   "MMM01",
   "POCKET CAMERA",
   "BANDAI TAMA5",
   "Huc3",
   "Huc1"
};

typedef struct
{
   catridge_type_enum type;
   bool RAM;
   bool BATTERY;
   bool TIMER;
   bool RUMBLE;
} cartridge_info_t;

static const cartridge_info_t cartridge_info_lut[0x100] =
{
   {CART_TYPE_GENERIC,                                false, false, false, false},
   {CART_TYPE_MBC1,                                   false, false, false, false},
   {CART_TYPE_MBC1,                                   true,  false, false, false},
   {CART_TYPE_MBC1,                                   true,  true,  false, false},
   {0},
   {CART_TYPE_MBC2,                                   false, false, false, false},
   {CART_TYPE_MBC2,                                   false, true,  false, false},
   {0},
   {CART_TYPE_GENERIC,                                true,  false, false, false},
   {CART_TYPE_GENERIC,                                true,  true,  false, false},
   {0},
   {CART_TYPE_MMM01,                                  false, false, false, false},
   {CART_TYPE_MMM01,                                  true,  false, false, false},
   {CART_TYPE_MMM01,                                  true,  true,  false, false},
   {0},
   {CART_TYPE_MBC3,                                   false, true,  true,  false},
   {CART_TYPE_MBC3,                                   true,  true,  true,  false},
   {CART_TYPE_MBC3,                                   false, false, false, false},
   {CART_TYPE_MBC3,                                   true,  false, false, false},
   {CART_TYPE_MBC3,                                   true,  true,  false, false},
   {0},
   {CART_TYPE_MBC4,                                   false, false, false, false},
   {CART_TYPE_MBC4,                                   true,  false, false, false},
   {CART_TYPE_MBC4,                                   true,  true,  false, false},
   {0},
   {CART_TYPE_MBC5,                                   false, false, false, false},
   {CART_TYPE_MBC5,                                   true,  false, false, false},
   {CART_TYPE_MBC5,                                   true,  true,  false, false},
   {CART_TYPE_MBC5,                                   false, false, false, true },
   {CART_TYPE_MBC5,                                   true,  false, false, true },
   {CART_TYPE_MBC5,                                   true,  true,  false, true },
   {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
   {CART_TYPE_POCKET_CAMERA,                          false, false, false, false},
   {CART_TYPE_BANDAI_TAMA5,                           false, false, false, false},
   {CART_TYPE_HuC3,                                   false, false, false, false},
   {CART_TYPE_HuC1,                                   true,  true,  false, false},
};

typedef struct
{
   uint8_t buffer0[0x100];
   uint8_t startup[0x4];
   uint8_t logo[0x30];
   union
   {
      uint8_t title[0x10];
      struct
      {
         uint8_t gbc_title[0xB];
         uint8_t gbc_manufacturer_code[0x4];
         uint8_t gbc_flag;
      };
   };
   uint8_t new_licencee_code[0x2];
   uint8_t sgb_flag;
   uint8_t cartridge_type;
   uint8_t rom_size;
   uint8_t ram_size;
   uint8_t destination_code;
   uint8_t old_licencee_code;
   uint8_t version_number;
   uint8_t header_checksum;
   uint8_t global_checksum_high;
   uint8_t global_checksum_low;
} gbemu_rom_header_t;

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
               unsigned FL :4;
               unsigned FC :1;
               unsigned FH :1;
               unsigned FN :1;
               unsigned FZ :1;
            };
            uint8_t F;
         };
         uint8_t A;
      };
      uint16_t AF;
   };
   union
   {
      struct
      {
         uint8_t C;
         uint8_t B;
      };
      uint16_t BC;
   };
   union
   {
      struct
      {
         uint8_t E;
         uint8_t D;
      };
      uint16_t DE;
   };
   union
   {
      struct
      {
         uint8_t L;
         uint8_t H;
      };
      uint16_t HL;
   };
   uint16_t SP;
   uint16_t PC;
}gbemu_cpu_t;

struct
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
      uint8_t MEMORY[0x10000];
   };
   uint8_t BIOS[0x100];
   gbemu_cpu_t CPU;
}GB;

static const int gbemu_ram_size_lut[4] = {0, 0x800, 0x2000, 0x8000};

int gbemu_get_rom_size(uint8_t rom_size_flag)
{
   return (0x8000 << (rom_size_flag & 0xF)) + ((rom_size_flag >> 4)? 0x100000: 0x0);
}

bool gbemu_load_game(const void* data, size_t size, const void* bios_data)
{
   int i;

   cartridge_info_t* cart_info = (cartridge_info_t*)&cartridge_info_lut[GB.HEADER.cartridge_type];

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

   printf("cart type : 0x%02X --> %s%s%s%s%s\n",(uint32_t)GB.HEADER.cartridge_type,
          cartridge_type_str[cart_info->type], cart_info->RAM?"+RAM":"",
          cart_info->BATTERY?"+BATTERY":"", cart_info->TIMER?"+TIMER":"",
          cart_info->RUMBLE?"+RUMBLE":"");
   printf("cart type : 0x%02X\n", (uint32_t)GB.HEADER.cartridge_type);
   printf("rom size : 0x%X --> 0x%X(%u, %u banks)\n", GB.HEADER.rom_size, gbemu_get_rom_size(GB.HEADER.rom_size), gbemu_get_rom_size(GB.HEADER.rom_size)
          , gbemu_get_rom_size(GB.HEADER.rom_size) >> 14);
   printf("ram size : %u\n", gbemu_ram_size_lut[GB.HEADER.ram_size]);
   printf("destination code : 0x%02X (%sJapanese)\n", (uint32_t)GB.HEADER.destination_code, GB.HEADER.destination_code?"non-":"");
   printf("old licencee code : 0x%02X\n", (uint32_t)GB.HEADER.old_licencee_code);
   printf("version number : 0x%02X\n", (uint32_t)GB.HEADER.version_number);
   printf("header checksum : 0x%02X\n", (uint32_t)GB.HEADER.header_checksum);
   printf("global checksum : 0x%02X%02X\n", (uint32_t)GB.HEADER.global_checksum_high
          , (uint32_t)GB.HEADER.global_checksum_low);



   fflush(stdout);
   exit(0);
}
