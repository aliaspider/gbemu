#ifndef PPU_H
#define PPU_H
#include <stdint.h>

#include "gbemu.h"

#define GBEMU_DRAWBUFFER_W 512
#define GBEMU_DRAWBUFFER_H 768

extern uint16_t gbemu_frame[GBEMU_DRAWBUFFER_W * GBEMU_DRAWBUFFER_H];

void gbemu_draw_bgmap(void);
void gbemu_draw_tilemap(void);
void gbemu_dump_memory(void);


void gbemu_ppu_draw(int cycles);
#endif // PPU_H
