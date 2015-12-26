
#include "gbemu.h"

#include <string.h>

uint16_t gbemu_frame[256 * 512];
uint16_t* const gbemu_tilemap_frame = &gbemu_frame[160*144];


void gbemu_draw_tilemap(void)
{
   memset(gbemu_tilemap_frame, 0xFF, 160*144 * sizeof(uint16_t));
   memcpy(gbemu_tilemap_frame, GB.VRAM, 0x2000);
   memcpy(gbemu_tilemap_frame + 0x1000, GB.WRAM, 0x2000);


}

void gbemu_dump_memory(void)
{
   memcpy(gbemu_frame, GB.MEMORY, 0x10000);

}
