
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
         *frame++;
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
   memcpy(gbemu_frame, GB.MEMORY, 0x10000);

}
