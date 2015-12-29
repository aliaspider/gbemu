
#include "gbemu.h"

#include <string.h>

uint16_t gbemu_frame[256 * 512];
uint16_t* const gbemu_tilemap_frame = &gbemu_frame[0x10000];

#if 1
uint16_t gbemu_palette[] = {0xFFFF,0x14|0x2A<<6|0x14<<11,
                            0xA|0x15<<6|0xA<<11,0x0000,};
#else
uint16_t gbemu_palette[] = {0xFFFF,0x14|0x2A<<5|0x14<<10,
                            0xA|0x15<<5|0xA<<10,0x0000,};
#endif

static void gbemu_draw_tile(uint8_t* tile, uint16_t* frame, int stride)
{
   int i,j;
   for (i=0; i<8; i++)
   {
      uint8_t bp0 = *tile++;
      uint8_t bp1 = *tile++;
      for (j=0; j<8; j++)
      {
         int id = ((bp0 >> 7) & 0x1) | ((bp1 >> 6) & 0x2);
         bp0<<=1;
         bp1<<=1;
         *frame= gbemu_palette[id];
         frame++;
      }
      frame += stride - 8;
   }

}


void gbemu_draw_tilemap(void)
{
   int i;
   memset(gbemu_tilemap_frame, 0x0, 160*144 * sizeof(uint16_t));
//   memcpy(gbemu_tilemap_frame, GB.VRAM, 0x2000);
//   memcpy(gbemu_tilemap_frame + 0x1000, GB.WRAM, 0x2000);

   uint8_t* ptr = GB.VRAM;
   uint16_t* dst = gbemu_tilemap_frame;
   i = 0;
   while (ptr < &GB.VRAM[0x2000])
   {
      gbemu_draw_tile(ptr, dst, 256);
      ptr+= 16;
      dst += 8;
      i++;
#if 0
      if(i==32)
      {
         dst += 7 * 256;
         i=0;
      }
#else
      if(i==16)
      {
         dst += 15 * 128;
         i=0;
      }
#endif
   }




}

void gbemu_dump_memory(void)
{
   int i;
   for (i = 0; i < 0x100; i++)
      memcpy(&gbemu_frame[0x10000 + 0x80 + (i << 8)], &GB.MEMORY[i << 8], 0x100);

}


void gbemu_ppu_draw(int cycles)
{
   static int last_cycles = 0;

   cycles *= 4;

   if(cycles < last_cycles)
      last_cycles = 0;

   int i;
   for (i = last_cycles; i < cycles ; i++)
   {
      int scanline = i / (4 * GB_LINE_TICK_COUNT);
      if(scanline > 143)
         goto finish;

      if(scanline == 43)
         fflush(stdout);

      int current = i - (scanline * 4 * GB_LINE_TICK_COUNT) - 80;

      if(current < 0 || current > 160)
         continue;

//      FF40 - LCDC - LCD Control (R/W)
//        Bit 7 - LCD Display Enable             (0=Off, 1=On)
//        Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
//        Bit 5 - Window Display Enable          (0=Off, 1=On)
//        Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
//        Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
//        Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
//        Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
//        Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)


//      Bit 0-2  Background Palette number  (BGP0-7)
//      Bit 3    Tile VRAM Bank number      (0=Bank 0, 1=Bank 1)
//      Bit 4    Not used
//      Bit 5    Horizontal Flip            (0=Normal, 1=Mirror horizontally)
//      Bit 6    Vertical Flip              (0=Normal, 1=Mirror vertically)
//      Bit 7    BG-to-OAM Priority         (0=Use OAM priority bit, 1=BG Priority)

      uint8_t* bg_tile_map = (GB.LCDC & 0x08)? &GB.VRAM[0x1C00]: &GB.VRAM[0x1800];
      uint8_t* bg_tile_data = (GB.LCDC & 0x10)? &GB.VRAM[0x1000]: &GB.VRAM[0x0000];


      uint8_t SCY = GB.MEMORY[0xFF42];
      uint8_t SCX = GB.MEMORY[0xFF43];

      uint8_t map_coord_y = scanline + SCY;
      uint8_t map_coord_x = current + SCX;

      int16_t tile_id = bg_tile_map[(map_coord_x / 8) + (map_coord_y / 8) * 32];

      tile_id = (GB.LCDC & 0x10)? (int8_t)tile_id: tile_id;
//      tile_id &= 0xFF;

      map_coord_x &= 0x7;
      map_coord_y &= 0x7;


      uint8_t* tile_data = &bg_tile_data[tile_id << 4];

      tile_data += (map_coord_y << 1);

      uint8_t bp0 = *tile_data << map_coord_x;
      uint8_t bp1 = *(tile_data + 1) << map_coord_x;
      int id = ((bp0 >> 7) & 0x1) | ((bp1 >> 6) & 0x2);
      gbemu_frame[current + scanline * 256] = gbemu_palette[id];

   }



finish:
   last_cycles = cycles;
}
