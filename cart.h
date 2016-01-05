#include <stdint.h>
#include <stdbool.h>


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

typedef struct
{
   catridge_type_enum type;
   bool RAM;
   bool BATTERY;
   bool TIMER;
   bool RUMBLE;
} cartridge_info_t;

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
   uint8_t cart_info_id;
   uint8_t rom_size_id;
   uint8_t ram_size_id;
   uint8_t destination_code;
   uint8_t old_licencee_code;
   uint8_t version_number;
   uint8_t header_checksum;
   uint8_t global_checksum_high;
   uint8_t global_checksum_low;
} gbemu_rom_header_t;

int gbemu_get_rom_size(uint8_t rom_size_id);
int gbemu_get_ram_size(uint8_t ram_size_id);

const cartridge_info_t* gbemu_get_cart_info(uint8_t cart_info_id);
const char* gbemu_get_cart_type_ident(catridge_type_enum cart_type_id);