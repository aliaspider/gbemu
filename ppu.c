
#include "gbemu.h"

#include <string.h>

uint16_t gbemu_frame[GBEMU_DRAWBUFFER_W* GBEMU_DRAWBUFFER_H];
uint16_t* const gbemu_tilemap_frame = &gbemu_frame[256];
uint16_t* const gbemu_bgmap_frame = &gbemu_frame[256 * GBEMU_DRAWBUFFER_W];
uint16_t* const gbemu_spritemap_frame = &gbemu_frame[256 * GBEMU_DRAWBUFFER_W + 256];

#if 0
uint16_t gbemu_palette[] = {0xFFFF, 0x14 | 0x2A << 6 | 0x14 << 11,
                            0xA | 0x15 << 6 | 0xA << 11, 0x0000,
                           };
#else
uint16_t gbemu_palette[] = {0xFFFF, 0x14 | 0x2A << 5 | 0x14 << 10,
                            0xA | 0x15 << 5 | 0xA << 10, 0x0000,
                           };
#endif


typedef struct __attribute((packed))
{
   uint8_t posY;
   uint8_t posX;
   uint8_t ID;
   struct __attribute((packed))
   {
      unsigned : 4;
      unsigned palette : 1;
      unsigned flipX   : 1;
      unsigned flipY   : 1;
      unsigned prio    : 1;
   };
}
gbemu_object_attr_t;


static void gbemu_draw_tile(uint8_t* tile, uint16_t* frame, int stride)
{
   int i, j;

   for (i = 0; i < 8; i++)
   {
      uint8_t bp0 = *tile++;
      uint8_t bp1 = *tile++;

      for (j = 0; j < 8; j++)
      {
         int id = ((bp0 >> 7) & 0x1) | ((bp1 >> 6) & 0x2);
         bp0 <<= 1;
         bp1 <<= 1;
         *frame = gbemu_palette[id];
         frame++;
      }

      frame += stride - 8;
   }

}


void gbemu_draw_tilemap(void)
{
   int i;
   //   memset(gbemu_tilemap_frame, 0x0, 160*144 * sizeof(uint16_t));
   //   memcpy(gbemu_tilemap_frame, GB.VRAM, 0x2000);
   //   memcpy(gbemu_tilemap_frame + 0x1000, GB.WRAM, 0x2000);

   uint8_t* ptr = GB.VRAM;
   uint16_t* dst = gbemu_tilemap_frame;
   i = 0;

   while (ptr < &GB.VRAM[0x2000])
   {
      gbemu_draw_tile(ptr, dst, GBEMU_DRAWBUFFER_W);
      ptr += 16;
      dst += 8;
      i++;
#if 0

      if (i == 32)
      {
         dst += 7 * 256;
         i = 0;
      }

#else

      if (i == 16)
      {
         dst += 8 * (GBEMU_DRAWBUFFER_W) - 128;
         i = 0;

         if (dst == &gbemu_tilemap_frame[GBEMU_DRAWBUFFER_W * 128])
            dst = &gbemu_tilemap_frame[128];
      }

#endif
   }




}

void gbemu_draw_bgmap(void)
{
   if (GB.LCDC.BG_WIN_data_select)
   {
      uint8_t* bg_tile_map = GB.LCDC.BG_tilemap_select ? &GB.VRAM[0x1C00] : &GB.VRAM[0x1800];
      uint8_t* bg_tile_data = &GB.VRAM[0x0000];
      int i, j;

      for (j = 0; j < 32; j++)
      {
         for (i = 0; i < 32; i++)
            gbemu_draw_tile(&bg_tile_data[bg_tile_map[i + j * 32] * 16], &gbemu_bgmap_frame[i * 8 + j * 8 * GBEMU_DRAWBUFFER_W],
                            GBEMU_DRAWBUFFER_W);
      }
   }
   else
   {
      int8_t* bg_tile_map = GB.LCDC.BG_tilemap_select ? (int8_t*)&GB.VRAM[0x1C00] : (int8_t*)&GB.VRAM[0x1800];
      uint8_t* bg_tile_data = &GB.VRAM[0x1000];
      int i, j;

      for (j = 0; j < 32; j++)
      {
         for (i = 0; i < 32; i++)
            gbemu_draw_tile(&bg_tile_data[bg_tile_map[i + j * 32] * 16], &gbemu_bgmap_frame[i * 8 + j * 8 * GBEMU_DRAWBUFFER_W],
                            GBEMU_DRAWBUFFER_W);
      }
   }

}

void gbemu_draw_sprite_map(void)
{
   int i;
   gbemu_object_attr_t* obj = (gbemu_object_attr_t*)GB.OAM;

   for (i = 0; i < 40; i++, obj++)
      gbemu_draw_tile(&GB.VRAM[obj->ID << 4], &gbemu_spritemap_frame[(i % 4) * 8 + (i / 4) * 8 * GBEMU_DRAWBUFFER_W],
                      GBEMU_DRAWBUFFER_W);

}

void gbemu_dump_memory(void)
{
   memcpy(&gbemu_frame[(256 + 256 + 160) * GBEMU_DRAWBUFFER_W], GB.MEMORY, 0x10000);
   //   int i;
   //   for (i = 0; i < 0x100; i++)
   //      memcpy(&gbemu_frame[0x10000 + 0x80 + (i << 8)], &GB.MEMORY[i << 8], 0x100);

}


void gbemu_ppu_draw(int cycles)
{
   static int last_cycles = 0;

   cycles *= 4;

   if (!GB.LCDC.LCD_enable)
   {
      last_cycles = cycles;
      return;
   }

   if (cycles < last_cycles)
      last_cycles = 0;


   int i;

   for (i = last_cycles; i < cycles ; i++)
   {
      int scanline = i / (4 * GB_LINE_TICK_COUNT);

      if (scanline > 143)
         goto finish;

      int current = i - (scanline * 4 * GB_LINE_TICK_COUNT) - 80;

      if (current < 0 || current > 159)
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


      uint8_t* bg_tile_map = GB.LCDC.BG_tilemap_select  ? &GB.VRAM[0x1C00] : &GB.VRAM[0x1800];
      uint8_t* win_tile_map = GB.LCDC.WIN_tilemap_select ? &GB.VRAM[0x1C00] : &GB.VRAM[0x1800];
      uint8_t* tile_data_vram = GB.LCDC.BG_WIN_data_select ? &GB.VRAM[0x0000] : &GB.VRAM[0x1000];


      uint8_t map_coord_x, map_coord_y;
      int16_t tile_id;

      if(GB.LCDC.WIN_enable && (GB.WY <= scanline) && ((GB.WX - 7) <= current))
      {
         map_coord_y = scanline - GB.WY;
         map_coord_x = current - (GB.WX - 7);

         tile_id = win_tile_map[(map_coord_x / 8) + (map_coord_y / 8) * 32];
      }
      else
      {
         map_coord_y = scanline + GB.SCY;
         map_coord_x = current + GB.SCX;

         tile_id = bg_tile_map[(map_coord_x / 8) + (map_coord_y / 8) * 32];
      }

      tile_id = GB.LCDC.BG_WIN_data_select ? tile_id : (int8_t)tile_id;
      map_coord_x &= 0x7;
      map_coord_y &= 0x7;


      uint8_t* tile_data = &tile_data_vram[(int)tile_id * 16];

      tile_data += (map_coord_y << 1);

      uint8_t bp0 = *tile_data << map_coord_x;
      uint8_t bp1 = *(tile_data + 1) << map_coord_x;
      int id = ((bp0 >> 7) & 0x1) | ((bp1 >> 6) & 0x2);
      id = (GB.BGP >> (id << 1)) & 0x3;
      gbemu_frame[current + scanline * GBEMU_DRAWBUFFER_W] = gbemu_palette[id];

      gbemu_object_attr_t* obj = (gbemu_object_attr_t*)GB.OAM;

      if(GB.LCDC.OBJ_enable)
      {
         do
         {
            if (obj->prio && id > 0)
               continue;

            int offsetX = current + 8 - (int)obj->posX;
            int offsetY = scanline + 16 - (int)obj->posY;

            if ((offsetX < 0) || (offsetX > 7) || (offsetY < 0) || (offsetY > (GB.LCDC.OBJ_size? 15 : 7)))
               continue;

            uint8_t* spr_tile_data = &GB.VRAM[GB.LCDC.OBJ_size? (obj->ID >> 1) << 5 : obj->ID << 4];
            spr_tile_data += (offsetY << 1);

            if(obj->flipX)
               offsetX = 7 - offsetX;
            if(obj->flipY)
               offsetY = (GB.LCDC.OBJ_size? 15 : 7) - offsetY;

            uint8_t spr_bp0 = *spr_tile_data << offsetX;
            uint8_t spr_bp1 = *(spr_tile_data + 1) << offsetX;
            int spr_id = ((spr_bp0 >> 7) & 0x1) | ((spr_bp1 >> 6) & 0x2);

            if (spr_id)
            {
               if (obj->palette)
                  spr_id = (GB.OBP1 >> (spr_id << 1)) & 0x3;
               else
                  spr_id = (GB.OBP0 >> (spr_id << 1)) & 0x3;

               gbemu_frame[current + scanline * GBEMU_DRAWBUFFER_W] = gbemu_palette[spr_id];
               break;
            }

         }
         while (++obj < (gbemu_object_attr_t*)&GB.OAM[0xA0]);
      }

   }

finish:
   last_cycles = cycles;
}
