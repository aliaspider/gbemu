
#include "cart.h"

static const char* cartridge_type_str[] =
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

int gbemu_get_rom_size(uint8_t rom_size_id)
{
   return (0x8000 << (rom_size_id & 0xF)) + ((rom_size_id >> 4)? 0x100000: 0x0);
}

int gbemu_get_ram_size(uint8_t ram_size_id)
{
   static const int gbemu_ram_size_lut[4] = {0, 0x800, 0x2000, 0x8000};
   if(ram_size_id >= sizeof(gbemu_ram_size_lut))
      return 0;
   return gbemu_ram_size_lut[ram_size_id];
}

const cartridge_info_t* gbemu_get_cart_info(uint8_t cart_info_id)
{
   return &cartridge_info_lut[cart_info_id];
}

const char* gbemu_get_cart_type_ident(catridge_type_enum cart_type_id)
{
   if(cart_type_id >= sizeof(cartridge_type_str))
      return "";

   return cartridge_type_str[cart_type_id];
}

