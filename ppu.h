#ifndef PPU_H
#define PPU_H

#include <stdint.h>

extern uint16_t gbemu_frame[256 * 512];

void gbemu_draw_tilemap(void);
void gbemu_dump_memory(void);

#endif // PPU_H
