#include "gbemu.h"
#include "cpudisasm.h"
#include "cpumacros.h"


#include "libretro.h"
#include <stdint.h>
#include <signal.h>
#include <string.h>

#define GB_LY  GB.MEMORY[0xFF44]
#define GB_LYC GB.MEMORY[0xFF45]


uint8_t gbemu_read_u8(uint16_t addr)
{

   if ((GB.MBC.type == CART_TYPE_MBC1)||(GB.MBC.type == CART_TYPE_MBC2))
   {
      if ((addr >= 0x4000) && (addr < 0x8000))
         return GB.MBC.active_ROM_bank[addr & 0x3FFF];
      else if ((addr >= 0xA000) && (addr < 0xC000))
      {
         if (GB.MBC.SRAM_enable)
            return GB.MBC.active_SRAM_bank[addr & 0x1FFF];
         else
            return 0x00;
      }
   }

   switch (addr)
   {
   case 0xFF00:
   {
      extern retro_input_state_t input_cb;

      typedef union __attribute((packed))
      {
         struct
         {
            unsigned A       : 1;
            unsigned B       : 1;
            unsigned select  : 1;
            unsigned start   : 1;
            unsigned         : 4;
         };
         struct
         {
            unsigned right   : 1;
            unsigned left    : 1;
            unsigned up      : 1;
            unsigned down    : 1;
            unsigned         : 4;
         };
         struct
         {
            unsigned                : 4;
            unsigned dpad_select    : 1;
            unsigned buttons_select : 1;
         };
         uint8_t val;
      }
      gbemu_pad_t;

      gbemu_pad_t gbemu_pad;

      gbemu_pad.val = GB.MEMORY[0xFF00];

      gbemu_pad.val |= 0xF;


      if (!gbemu_pad.dpad_select)
      {
         gbemu_pad.right = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
         gbemu_pad.left = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
         gbemu_pad.up = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
         gbemu_pad.down = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
      }

      if (!gbemu_pad.buttons_select)
      {
         gbemu_pad.A = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
         gbemu_pad.B = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
         gbemu_pad.select = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
         gbemu_pad.start = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
      }


      if (!gbemu_pad.A)
         fflush(stdout);

      GB.MEMORY[0xFF00] = gbemu_pad.val;
      return GB.MEMORY[0xFF00];
   }

   case 0xFF10: //NR10
      return GB.MEMORY[0xFF10] | 0x80;
   case 0xFF11: //NR11
   case 0xFF16: //NR21
      return GB.MEMORY[addr] | 0x3F;
   case 0xFF14: //NR14
   case 0xFF19: //NR24
   case 0xFF1E: //NR34
   case 0xFF23: //NR44
      return GB.MEMORY[addr] | 0xBF;
   case 0xFF1A: //NR30
      return GB.MEMORY[0xFF1A] | 0x7F;
   case 0xFF1C: //NR32
      return GB.MEMORY[0xFF1C] | 0x9F;
   case 0xFF26: //NR52
      return GB.MEMORY[0xFF26] | 0x70;
   case 0xFF13: //NR13
   case 0xFF15: //NR20
   case 0xFF18: //NR23
   case 0xFF1B: //NR31
   case 0xFF1D: //NR33
   case 0xFF1F: //NR40
   case 0xFF20: //NR41
   case 0xFF27:
   case 0xFF28:
   case 0xFF29:
   case 0xFF2A:
   case 0xFF2B:
   case 0xFF2C:
   case 0xFF2D:
   case 0xFF2E:
   case 0xFF2F:
      return 0xFF;




   default:
      return GB.MEMORY[addr];
   }
}

void gbemu_write_u8(uint16_t addr, uint8_t val)
{
   if ((GB.MBC.type == CART_TYPE_MBC1) && (addr < 0x8000))
   {
      if (addr < 0x2000)
         GB.MBC.SRAM_enable = ((val & 0xF) == 0xA);
      else if (addr < 0x4000)
      {
         GB.MBC.bank_id_low = val & 0x1F;

         if (!GB.MBC.bank_id_low)
            GB.MBC.bank_id_low = 0x1;

         if (GB.MBC.SRAM_banking_mode)
            GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[0][GB.MBC.bank_id_low];
         else
            GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[GB.MBC.bank_id_high][GB.MBC.bank_id_low];

      }
      else if (addr < 0x6000)
      {
         GB.MBC.bank_id_high = val & 0x3;
         if (GB.MBC.SRAM_banking_mode)
         {
            GB.MBC.active_SRAM_bank = GB.MBC.SRAM_banks[GB.MBC.bank_id_high];
//            GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[0][GB.MBC.bank_id_low];
         }
         else
         {
//            GB.MBC.active_SRAM_bank = GB.MBC.SRAM_banks[0];
            GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[GB.MBC.bank_id_high][GB.MBC.bank_id_low];
         }

      }
      else
      {
         GB.MBC.SRAM_banking_mode = val & 0x1;
         if (GB.MBC.SRAM_banking_mode)
         {
            GB.MBC.active_SRAM_bank = GB.MBC.SRAM_banks[GB.MBC.bank_id_high];
            GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[0][GB.MBC.bank_id_low];
         }
         else
         {
            GB.MBC.active_SRAM_bank = GB.MBC.SRAM_banks[0];
            GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[GB.MBC.bank_id_high][GB.MBC.bank_id_low];
         }
      }

      return;
   }
   else if ((GB.MBC.type == CART_TYPE_MBC2) && (addr < 0x4000))
   {
      if (addr < 0x2000)
      {
         if(!(addr & 0x100))
            GB.MBC.SRAM_enable = ((val & 0xF) == 0xA);
      }
      else if((addr & 0x100))
         GB.MBC.active_ROM_bank = GB.MBC.ROM_banks[0][val & 0xF];

      return;
   }

   switch (addr)
   {
   case 0xFF00:
      GB.MEMORY[0xFF00] = (val & 0xF0) | (GB.MEMORY[0xFF00] & 0xF);
      return;

   case 0xFF02:
      GB.MEMORY[0xFF02] = val;
      if(val == 0x81)
      {
         GB.serial_port.buffer[GB.serial_port.write_index++] = GB.SB;
         GB.serial_port.write_index &= 0xFFF;
      }
      return;
   case 0xFF04:
      GB.DIV = 0x00;
      return;
   case 0xFF46:
      memcpy(GB.OAM, &GB.MEMORY[val << 8], 0xA0);
      return;
   case 0xFF11: //NR11
      GB.MEMORY[0xFF11] = val;
      GB.SND_regs.channels.master.L_square1_enable = 1;
      return;      
   case 0xFF12: //NR12
      GB.MEMORY[0xFF12] = val;
      return;
   case 0xFF13: //NR13
      GB.MEMORY[0xFF13] = val;
      return;
   case 0xFF14: //NR14
      GB.MEMORY[0xFF14] = val;
      if (val & 0x80)
      {
         GB.APU.square1.envelope.counter = GB.SND_regs.channels.square1.envelope_period;
         GB.APU.square1.envelope.volume = GB.SND_regs.channels.square1.envelope_starting_volume;
         GB.APU.square1.envelope.increment = GB.SND_regs.channels.square1.envelope_add_mode;

         GB.APU.square1.sweep.frequency = GB.SND_regs.channels.square1.frequency;
         GB.APU.square1.sweep.counter = GB.SND_regs.channels.square1.sweep_period;
         GB.APU.square1.sweep.enabled = (GB.SND_regs.channels.square1.sweep_period && GB.SND_regs.channels.square1.sweep_shift);

         GB.SND_regs.channels.master.L_square1_enable = 1;
         GB.SND_regs.channels.square1.length_load = 0;
      }
      return;
   case 0xFF16: //NR21
      GB.MEMORY[0xFF16] = val;
      GB.SND_regs.channels.master.L_square2_enable = 1;
      return;
   case 0xFF19: //NR24
      GB.MEMORY[0xFF19] = val;
      if (val & 0x80)
      {
         GB.APU.square2.envelope.counter = GB.SND_regs.channels.square2.envelope_period;
         GB.APU.square2.envelope.volume = GB.SND_regs.channels.square2.envelope_starting_volume;
         GB.APU.square2.envelope.increment = GB.SND_regs.channels.square2.envelope_add_mode;

         GB.SND_regs.channels.master.L_square2_enable = 1;
         GB.SND_regs.channels.square2.length_load = 0;
      }
      return;
   case 0xFF1B: //NR31
      GB.MEMORY[0xFF1B] = val;
      GB.SND_regs.channels.master.L_wave_enable = 1;
      return;
   case 0xFF1E: //NR34
      GB.MEMORY[0xFF1E] = val;
      if (val & 0x80)
      {
         GB.SND_regs.channels.master.L_wave_enable = 1;
         GB.SND_regs.channels.wave.length_load = 0;
      }
      return;
   case 0xFF20: //NR41
      GB.MEMORY[0xFF20] = val;
   GB.SND_regs.channels.master.L_noise_enable = 1;
      return;
   case 0xFF22: //NR43
      GB.MEMORY[0xFF22] = val;
      return;
   case 0xFF23: //NR44
      GB.MEMORY[0xFF23] = val;
      if (val & 0x80)
      {
         GB.APU.noise.envelope.counter = GB.SND_regs.channels.noise.envelope_period;
         GB.APU.noise.envelope.volume = GB.SND_regs.channels.noise.envelope_starting_volume;
         GB.APU.noise.envelope.increment = GB.SND_regs.channels.noise.envelope_add_mode;

         GB.SND_regs.channels.master.L_noise_enable = 1;
         GB.SND_regs.channels.noise.length_load = 0;

         GB.APU.noise.PRNG = 0x7FFF;
         GB.APU.noise.shift_counter = 1 << GB.SND_regs.channels.noise.clock_shift;
         GB.APU.noise.down_counter = GB.SND_regs.channels.noise.divisor_code;

         GB.APU.noise.counter--;
         unsigned s = (GB.SND_regs.channels.noise.clock_shift) + 3;
         unsigned r = GB.SND_regs.channels.noise.divisor_code;

         if (!r) {
            r = 1;
            --s;
         }

         GB.APU.noise.counter = (r << s);
      }
      return;
   case 0xFF44:
      return;
   default:
      if (addr < 0x8000)
         return;
      else if ((addr >= 0xA000) && (addr < 0xC000))
      {
         if (GB.MBC.SRAM_enable)
            GB.MBC.active_SRAM_bank[addr & 0x1FFF] = val;
      }
      else if ((addr >= 0xC000) && (addr < 0xDE00))
      {
         GB.MEMORY[addr] = val;
         GB.MEMORY[addr + 0x2000] = val;
      }
      else
         GB.MEMORY[addr] = val;
   }
}

int8_t gbemu_read_s8(uint16_t addr)
{
   return (int8_t)gbemu_read_u8(addr);
}

void gbemu_cpu_run(int cycles)
{
   gbemu_cpu_t CPU = GB.CPU;

   CPU.cycles = 0;
   GB.APU.cycles = 0;
   GB.APU.write_pos = gbemu_sound_buffer;
   int cycles_last = 0;
   static int h_cycles = 0;
next_instruction:

   gbemu_ppu_draw(CPU.cycles);
   gbemu_apu_run(CPU.cycles);

   CPU.timer.ticks += CPU.cycles - cycles_last;

   while (CPU.timer.ticks_last < CPU.timer.ticks)
   {
      CPU.timer.ticks_last++;
      if (!(CPU.timer.ticks_last & 0x3F))
         GB.DIV++;

      if (GB.TAC.active)
      {
         /* 0: 0xFF
          * 1: 0x03
          * 2: 0x0F
          * 3: 0x3F */
         static const uint8_t timer_masks[4] = {0xFF, 0x03, 0x0F, 0x3F};
         if (!(CPU.timer.ticks_last & timer_masks[GB.TAC.clock_select]))
         {
            GB.TIMA++;
            if (!GB.TIMA)
            {
               GB.TIMA = GB.TMA;
               GB.IF.timer = 1;
            }
         }
      }
   }

   if (CPU.cycles > cycles)
      goto cpu_exit;

   h_cycles += CPU.cycles - cycles_last;
   cycles_last = CPU.cycles;
   if (h_cycles > GB_LINE_TICK_COUNT)
   {
      h_cycles -= GB_LINE_TICK_COUNT;
      GB_LY++;
      if (GB_LY >= GB_V_COUNT)
      {
         GB_LY = 0;
         goto cpu_exit;
      }
   }



   if (GB.LCDC.LCD_enable)
   {
      if (GB_LY == GB_LYC)
      {
         GB.LCD_STAT.LCY_eq_LY_flag = 1;
         if(GB.LCD_STAT.LCY_eq_LY_IE)
            GB.IF.LCD_stat = 1;
      }
      if (GB_LY == 144)
      {
         GB.IF.Vblank = 1;
         if(GB.LCD_STAT.VBlank_IE)
            GB.IF.LCD_stat = 1;
      }

      if (GB_LY > 143)
         GB.LCD_STAT.mode_flag = GB_LCD_STAT_MODE1_VBLANK;
      else if (h_cycles > (42 + 20))
      {
         GB.LCD_STAT.mode_flag = GB_LCD_STAT_MODE0_HBLANK;
         if(GB.LCD_STAT.HBlank_IE)
            GB.IF.LCD_stat = 1;
      }
      else if (h_cycles > 20)
      {
         GB.LCD_STAT.mode_flag = GB_LCD_STAT_MODE3_OAM_VRAM_busy;
      }
      else
      {
         GB.LCD_STAT.mode_flag = GB_LCD_STAT_MODE2_OAM_busy;
         if(GB.LCD_STAT.OAM_IE)
            GB.IF.LCD_stat = 1;
      }

   }

//   if((GB.LCD_STAT.VBlank_IE && (GB.LCD_STAT.mode_flag == GB_LCD_STAT_MODE1_VBLANK)) ||
//      (GB.LCD_STAT.HBlank_IE && (GB.LCD_STAT.mode_flag == GB_LCD_STAT_MODE0_HBLANK)) ||
//      (GB.LCD_STAT.OAM_IE && (GB.LCD_STAT.mode_flag == GB_LCD_STAT_MODE2_OAM_busy)) ||
//      (GB.LCD_STAT.LCY_eq_LY_IE && GB.LCD_STAT.LCY_eq_LY_flag))
//      GB.IF.LCD_stat = 1;
   if(CPU.HALT)
   {
      if (GB.IF.Vblank
          || GB.IF.LCD_stat
          || GB.IF.timer
          || GB.IF.serial
          || GB.IF.joypad )
         CPU_disable_halt();
      else
      {
         CPU_cycles_inc();
         goto next_instruction;
      }
   }

   if (CPU.IME)
   {
      if ((GB.IF.Vblank && GB.IE.Vblank) && GB.LCDC.LCD_enable)
      {
         CPU.IME = 0;
         GB.IF.Vblank = 0;
         CPU_INT(0x40);
      }
      else if (GB.IF.LCD_stat && GB.IE.LCD_stat)
      {
         CPU.IME = 0;
         GB.IF.LCD_stat = 0;
         CPU_INT(0x48);
      }
      else if (GB.IF.timer && GB.IE.timer)
      {
         CPU.IME = 0;
         GB.IF.timer = 0;
         CPU_INT(0x50);
      }
      else if (GB.IF.serial && GB.IE.serial)
      {
         CPU.IME = 0;
         GB.IF.serial = 0;
         CPU_INT(0x58);
      }
      else if (GB.IF.joypad && GB.IE.joypad)
      {
         CPU.IME = 0;
         GB.IF.joypad = 0;
         CPU_INT(0x60);
      }
   }

//#define DISASM
   #define SKIP_COUNT 0x12000
   //#define SKIP_COUNT 0xEEE9
//   #define SKIP_COUNT 0x00049B4C
//#define SKIP_COUNT 0xFFFFFFFF
//#define SKIP_COUNT 0x00000000


   static int total_exec = 0;
#ifdef DISASM
   static bool force_disasm = false;
#endif
next_instruction_nocheck:
#ifdef DISASM
//      if(CPU.PC == 0xC2B5)
//   if (CPU.PC >= 0x4000)
//      force_disasm = true;

   if (total_exec > SKIP_COUNT)
   {
      force_disasm = true;
      printf("0x%08X: ", total_exec);
   }

   if (force_disasm)
   {
      gbemu_disasm_current(&CPU, true);
      fflush(stdout);
   }
#endif

   total_exec++;
//   if(GB.MEMORY[0xFF44] == 0x94)
//      fflush(stdout);
//   if ((CPU.AF == 0x810b))
//       fflush(stdout);
//   if ((CPU.PC == 0xDEF8))
//       fflush(stdout);
//   if ((CPU.PC == 0xC4C2) && (CPU.A == 0xF1))
//       fflush(stdout);


#ifdef USE_BIOS
   if(CPU.PC == 0x100)
      memcpy(GB.MEMORY, GB.BIOS, 0x100);
#endif

   switch (GB_READ_PC())
   {
   // NOP
   //   case 0x7F:
   //   case 0x40:
   //   case 0x49:
   //   case 0x52:
   //   case 0x5B:
   //   case 0x64:
   //   case 0x6D:
   case 0x00:
      CPU_NOP();
   case 0x08:
      CPU_LD_addr16_SP();
   case 0x10:
      CPU_STOP();
   case 0x18:
      CPU_JR(CPU_COND_ALWAYS);
   case 0x20:
      CPU_JR(CPU_COND_NZ);
   case 0x28:
      CPU_JR(CPU_COND_Z);
   case 0x30:
      CPU_JR(CPU_COND_NC);
   case 0x38:
      CPU_JR(CPU_COND_C);

   case 0x01:
      CPU_LD_rr_imm16(REG_BC);
   case 0x11:
      CPU_LD_rr_imm16(REG_DE);
   case 0x21:
      CPU_LD_rr_imm16(REG_HL);
   case 0x31:
      CPU_LD_rr_imm16(REG_SP);

   case 0x09:
      CPU_ADD_rr_rr(REG_HL, REG_BC);
   case 0x19:
      CPU_ADD_rr_rr(REG_HL, REG_DE);
   case 0x29:
      CPU_ADD_rr_rr(REG_HL, REG_HL);
   case 0x39:
      CPU_ADD_rr_rr(REG_HL, REG_SP);

   case 0x02:
      CPU_LD_raddr_r(REG_BC, REG_A);
   case 0x0A:
      CPU_LD_r_raddr(REG_A, REG_BC);
   case 0x12:
      CPU_LD_raddr_r(REG_DE, REG_A);
   case 0x1A:
      CPU_LD_r_raddr(REG_A, REG_DE);
   case 0x22:
      CPU_LD_raddr_r(REG_HL++, REG_A);
   case 0x2A:
      CPU_LD_r_raddr(REG_A, REG_HL++);
   case 0x32:
      CPU_LD_raddr_r(REG_HL--, REG_A);
   case 0x3A:
      CPU_LD_r_raddr(REG_A, REG_HL--);

   case 0x03:
      CPU_INC_rr(REG_BC);
   case 0x0B:
      CPU_DEC_rr(REG_BC);
   case 0x13:
      CPU_INC_rr(REG_DE);
   case 0x1B:
      CPU_DEC_rr(REG_DE);
   case 0x23:
      CPU_INC_rr(REG_HL);
   case 0x2B:
      CPU_DEC_rr(REG_HL);
   case 0x33:
      CPU_INC_rr(REG_SP);
   case 0x3B:
      CPU_DEC_rr(REG_SP);

   case 0x04:
      CPU_INC_r(REG_B);
   case 0x0C:
      CPU_INC_r(REG_C);
   case 0x14:
      CPU_INC_r(REG_D);
   case 0x1C:
      CPU_INC_r(REG_E);
   case 0x24:
      CPU_INC_r(REG_H);
   case 0x2C:
      CPU_INC_r(REG_L);
   case 0x34:
      CPU_INC_raddr(REG_HL);
   case 0x3C:
      CPU_INC_r(REG_A);

   case 0x05:
      CPU_DEC_r(REG_B);
   case 0x0D:
      CPU_DEC_r(REG_C);
   case 0x15:
      CPU_DEC_r(REG_D);
   case 0x1D:
      CPU_DEC_r(REG_E);
   case 0x25:
      CPU_DEC_r(REG_H);
   case 0x2D:
      CPU_DEC_r(REG_L);
   case 0x35:
      CPU_DEC_raddr(REG_HL);
   case 0x3D:
      CPU_DEC_r(REG_A);

   case 0x06:
      CPU_LD_r_imm8(REG_B);
   case 0x0E:
      CPU_LD_r_imm8(REG_C);
   case 0x16:
      CPU_LD_r_imm8(REG_D);
   case 0x1E:
      CPU_LD_r_imm8(REG_E);
   case 0x26:
      CPU_LD_r_imm8(REG_H);
   case 0x2E:
      CPU_LD_r_imm8(REG_L);
   case 0x36:
      CPU_LD_raddr_imm8(REG_HL);
   case 0x3E:
      CPU_LD_r_imm8(REG_A);

   case 0x07:
      CPU_RLCA();
   case 0x0F:
      CPU_RRCA();
   case 0x17:
      CPU_RLA();
   case 0x1F:
      CPU_RRA();
   case 0x27:
      CPU_DAA();
   case 0x2F:
      CPU_CPL();
   case 0x37:
      CPU_SCF();
   case 0x3F:
      CPU_CCF();
      
   // LD r1, r2
   case 0x40:
      CPU_LD_r0_r1(CPU.B, CPU.B);
   case 0x41:
      CPU_LD_r0_r1(CPU.B, CPU.C);
   case 0x42:
      CPU_LD_r0_r1(CPU.B, CPU.D);
   case 0x43:
      CPU_LD_r0_r1(CPU.B, CPU.E);
   case 0x44:
      CPU_LD_r0_r1(CPU.B, CPU.H);
   case 0x45:
      CPU_LD_r0_r1(CPU.B, CPU.L);
   case 0x46:
      CPU_LD_r_raddr(CPU.B, CPU.HL);
   case 0x47:
      CPU_LD_r0_r1(CPU.B, CPU.A);

   case 0x48:
      CPU_LD_r0_r1(CPU.C, CPU.B);
   case 0x49:
      CPU_LD_r0_r1(CPU.C, CPU.C);
   case 0x4A:
      CPU_LD_r0_r1(CPU.C, CPU.D);
   case 0x4B:
      CPU_LD_r0_r1(CPU.C, CPU.E);
   case 0x4C:
      CPU_LD_r0_r1(CPU.C, CPU.H);
   case 0x4D:
      CPU_LD_r0_r1(CPU.C, CPU.L);
   case 0x4E:
      CPU_LD_r_raddr(CPU.C, CPU.HL);
   case 0x4F:
      CPU_LD_r0_r1(CPU.C, CPU.A);

   case 0x50:
      CPU_LD_r0_r1(CPU.D, CPU.B);
   case 0x51:
      CPU_LD_r0_r1(CPU.D, CPU.C);
   case 0x52:
      CPU_LD_r0_r1(CPU.D, CPU.D);
   case 0x53:
      CPU_LD_r0_r1(CPU.D, CPU.E);
   case 0x54:
      CPU_LD_r0_r1(CPU.D, CPU.H);
   case 0x55:
      CPU_LD_r0_r1(CPU.D, CPU.L);
   case 0x56:
      CPU_LD_r_raddr(CPU.D, CPU.HL);
   case 0x57:
      CPU_LD_r0_r1(CPU.D, CPU.A);

   case 0x58:
      CPU_LD_r0_r1(CPU.E, CPU.B);
   case 0x59:
      CPU_LD_r0_r1(CPU.E, CPU.C);
   case 0x5A:
      CPU_LD_r0_r1(CPU.E, CPU.D);
   case 0x5B:
      CPU_LD_r0_r1(CPU.E, CPU.E);
   case 0x5C:
      CPU_LD_r0_r1(CPU.E, CPU.H);
   case 0x5D:
      CPU_LD_r0_r1(CPU.E, CPU.L);
   case 0x5E:
      CPU_LD_r_raddr(CPU.E, CPU.HL);
   case 0x5F:
      CPU_LD_r0_r1(CPU.E, CPU.A);

   case 0x60:
      CPU_LD_r0_r1(CPU.H, CPU.B);
   case 0x61:
      CPU_LD_r0_r1(CPU.H, CPU.C);
   case 0x62:
      CPU_LD_r0_r1(CPU.H, CPU.D);
   case 0x63:
      CPU_LD_r0_r1(CPU.H, CPU.E);
   case 0x64:
      CPU_LD_r0_r1(CPU.H, CPU.H);
   case 0x65:
      CPU_LD_r0_r1(CPU.H, CPU.L);
   case 0x66:
      CPU_LD_r_raddr(CPU.H, CPU.HL);
   case 0x67:
      CPU_LD_r0_r1(CPU.H, CPU.A);

   case 0x68:
      CPU_LD_r0_r1(CPU.L, CPU.B);
   case 0x69:
      CPU_LD_r0_r1(CPU.L, CPU.C);
   case 0x6A:
      CPU_LD_r0_r1(CPU.L, CPU.D);
   case 0x6B:
      CPU_LD_r0_r1(CPU.L, CPU.E);
   case 0x6C:
      CPU_LD_r0_r1(CPU.L, CPU.H);
   case 0x6D:
      CPU_LD_r0_r1(CPU.L, CPU.L);
   case 0x6E:
      CPU_LD_r_raddr(CPU.L, CPU.HL);
   case 0x6F:
      CPU_LD_r0_r1(CPU.L, CPU.A);

   case 0x70:
      CPU_LD_raddr_r(CPU.HL, CPU.B);
   case 0x71:
      CPU_LD_raddr_r(CPU.HL, CPU.C);
   case 0x72:
      CPU_LD_raddr_r(CPU.HL, CPU.D);
   case 0x73:
      CPU_LD_raddr_r(CPU.HL, CPU.E);
   case 0x74:
      CPU_LD_raddr_r(CPU.HL, CPU.H);
   case 0x75:
      CPU_LD_raddr_r(CPU.HL, CPU.L);
   case 0x76:
      CPU_HALT();
   case 0x77:
      CPU_LD_raddr_r(CPU.HL, CPU.A);

   case 0x78:
      CPU_LD_r0_r1(CPU.A, CPU.B);
   case 0x79:
      CPU_LD_r0_r1(CPU.A, CPU.C);
   case 0x7A:
      CPU_LD_r0_r1(CPU.A, CPU.D);
   case 0x7B:
      CPU_LD_r0_r1(CPU.A, CPU.E);
   case 0x7C:
      CPU_LD_r0_r1(CPU.A, CPU.H);
   case 0x7D:
      CPU_LD_r0_r1(CPU.A, CPU.L);
   case 0x7E:
      CPU_LD_r_raddr(CPU.A, CPU.HL);
   case 0x7F:
      CPU_LD_r0_r1(CPU.A, CPU.A);

   /* ALU */
   case 0x80:
      CPU_ADD_r_r(REG_A, REG_B);
   case 0x81:
      CPU_ADD_r_r(REG_A, REG_C);
   case 0x82:
      CPU_ADD_r_r(REG_A, REG_D);
   case 0x83:
      CPU_ADD_r_r(REG_A, REG_E);
   case 0x84:
      CPU_ADD_r_r(REG_A, REG_H);
   case 0x85:
      CPU_ADD_r_r(REG_A, REG_L);
   case 0x86:
      CPU_ADD_r_raddr(REG_A, REG_HL);
   case 0x87:
      CPU_ADD_r_r(REG_A, REG_A);

   case 0x88:
      CPU_ADC_r_r(REG_A, REG_B);
   case 0x89:
      CPU_ADC_r_r(REG_A, REG_C);
   case 0x8A:
      CPU_ADC_r_r(REG_A, REG_D);
   case 0x8B:
      CPU_ADC_r_r(REG_A, REG_E);
   case 0x8C:
      CPU_ADC_r_r(REG_A, REG_H);
   case 0x8D:
      CPU_ADC_r_r(REG_A, REG_L);
   case 0x8E:
      CPU_ADC_r_raddr(REG_A, REG_HL);
   case 0x8F:
      CPU_ADC_r_r(REG_A, REG_A);

   case 0x90:
      CPU_SUB_r_r(REG_A, REG_B);
   case 0x91:
      CPU_SUB_r_r(REG_A, REG_C);
   case 0x92:
      CPU_SUB_r_r(REG_A, REG_D);
   case 0x93:
      CPU_SUB_r_r(REG_A, REG_E);
   case 0x94:
      CPU_SUB_r_r(REG_A, REG_H);
   case 0x95:
      CPU_SUB_r_r(REG_A, REG_L);
   case 0x96:
      CPU_SUB_r_raddr(REG_A, REG_HL);
   case 0x97:
      CPU_SUB_r_r(REG_A, REG_A);

   case 0x98:
      CPU_SBC_r_r(REG_A, REG_B);
   case 0x99:
      CPU_SBC_r_r(REG_A, REG_C);
   case 0x9A:
      CPU_SBC_r_r(REG_A, REG_D);
   case 0x9B:
      CPU_SBC_r_r(REG_A, REG_E);
   case 0x9C:
      CPU_SBC_r_r(REG_A, REG_H);
   case 0x9D:
      CPU_SBC_r_r(REG_A, REG_L);
   case 0x9E:
      CPU_SBC_r_raddr(REG_A, REG_HL);
   case 0x9F:
      CPU_SBC_r_r(REG_A, REG_A);

   case 0xA0:
      CPU_AND_A_r(REG_B);
   case 0xA1:
      CPU_AND_A_r(REG_C);
   case 0xA2:
      CPU_AND_A_r(REG_D);
   case 0xA3:
      CPU_AND_A_r(REG_E);
   case 0xA4:
      CPU_AND_A_r(REG_H);
   case 0xA5:
      CPU_AND_A_r(REG_L);
   case 0xA6:
      CPU_AND_A_raddr(REG_HL);
   case 0xA7:
      CPU_AND_A_r(REG_A);

   case 0xA8:
      CPU_XOR_A_r(REG_B);
   case 0xA9:
      CPU_XOR_A_r(REG_C);
   case 0xAA:
      CPU_XOR_A_r(REG_D);
   case 0xAB:
      CPU_XOR_A_r(REG_E);
   case 0xAC:
      CPU_XOR_A_r(REG_H);
   case 0xAD:
      CPU_XOR_A_r(REG_L);
   case 0xAE:
      CPU_XOR_A_raddr(REG_HL);
   case 0xAF:
      CPU_XOR_A_r(REG_A);

   case 0xB0:
      CPU_OR_A_r(REG_B);
   case 0xB1:
      CPU_OR_A_r(REG_C);
   case 0xB2:
      CPU_OR_A_r(REG_D);
   case 0xB3:
      CPU_OR_A_r(REG_E);
   case 0xB4:
      CPU_OR_A_r(REG_H);
   case 0xB5:
      CPU_OR_A_r(REG_L);
   case 0xB6:
      CPU_OR_A_raddr(REG_HL);
   case 0xB7:
      CPU_OR_A_r(REG_A);

   case 0xB8:
      CPU_CP_A_r(REG_B);
   case 0xB9:
      CPU_CP_A_r(REG_C);
   case 0xBA:
      CPU_CP_A_r(REG_D);
   case 0xBB:
      CPU_CP_A_r(REG_E);
   case 0xBC:
      CPU_CP_A_r(REG_H);
   case 0xBD:
      CPU_CP_A_r(REG_L);
   case 0xBE:
      CPU_CP_A_raddr(REG_HL);
   case 0xBF:
      CPU_CP_A_r(REG_A);

   /*******/
   case 0xC0:
      CPU_RET_cc(CPU_COND_NZ);
   case 0xC8:
      CPU_RET_cc(CPU_COND_Z);
   case 0xD0:
      CPU_RET_cc(CPU_COND_NC);
   case 0xD8:
      CPU_RET_cc(CPU_COND_C);

   case 0xE0:
      CPU_LD_addr8_r(REG_A);
   case 0xF0:
      CPU_LD_r_addr8(REG_A);
   case 0xE8:      
      CPU_ADD_SP_off8();
   case 0xF8:
      CPU_LD_HL_SP_off8();

   // POP (rr)
   case 0xC1:
      CPU_POP_BC();
   case 0xD1:
      CPU_POP_DE();
   case 0xE1:
      CPU_POP_HL();
   case 0xF1:
      CPU_POP_AF();

   case 0xC9:
      CPU_RET();
   case 0xD9:
      CPU_RETI();
   case 0xE9:
      CPU_JP_HL();


   case 0xC2:
      CPU_JP(CPU_COND_NZ);
   case 0xD2:
      CPU_JP(CPU_COND_NC);
   case 0xCA:
      CPU_JP(CPU_COND_Z);
   case 0xDA:
      CPU_JP(CPU_COND_C);

   case 0xC3:
      CPU_JP(CPU_COND_ALWAYS);

   case 0xF3:
      CPU_DI();
   case 0xFB:
      CPU_EI();

   case 0xC4:
      CPU_CALL(CPU_COND_NZ);
   case 0xCC:
      CPU_CALL(CPU_COND_Z);
   case 0xD4:
      CPU_CALL(CPU_COND_NC);
   case 0xDC:
      CPU_CALL(CPU_COND_C);

   case 0xCD:
      CPU_CALL(CPU_COND_ALWAYS);



   // PUSH (rr)
   case 0xC5:
      CPU_PUSH_BC();
   case 0xD5:
      CPU_PUSH_DE();
   case 0xE5:
      CPU_PUSH_HL();
   case 0xF5:
      CPU_PUSH_AF();


   case 0xC6:
      CPU_ADD_r_imm8(REG_A);
   case 0xCE: // 0x0210 0xC674 0xDEF8
//      if(REG_PC == 0xDEF9)
//         fflush(stdout);
      CPU_ADC_r_imm8(REG_A);
   case 0xD6:
      CPU_SUB_r_imm8(REG_A);
   case 0xDE:
      CPU_SBC_r_imm8(REG_A);
   case 0xE6:
      CPU_AND_A_imm8();
   case 0xEE:
      CPU_XOR_A_imm8();
   case 0xF6:
      CPU_OR_A_imm8();
   case 0xFE:
      CPU_CP_A_imm8();

   case 0xFA:
   {
      uint16_t addr = GB_READ_U8(CPU.PC++);
      addr |= GB_READ_U8(CPU.PC++) << 8;
      CPU.A = GB_READ_U8(addr);
      CPU.cycles += 4;
      CPU_exec_next();
   }

   case 0xEA:
      CPU_LD_addr16_A();
   // LD A,(C)
   case 0xF2:
      CPU.A = GB_READ_U8(CPU.C | 0xFF00);
      CPU.cycles += 2;
      CPU_exec_next();
   // LD (C), A
   case 0xE2:
      GB_WRITE_U8((CPU.C | 0xFF00), CPU.A);
      CPU.cycles += 2;
      CPU_exec_next();

   // LD SP, HL
   case 0xF9:
      CPU.SP = CPU.HL;
      CPU.cycles += 2;
      CPU_exec_next();

   case 0xC7:
      CPU_RST(0x00);
   case 0xD7:
      CPU_RST(0x10);
   case 0xE7:
      CPU_RST(0x20);
   case 0xF7:
      CPU_RST(0x30);

   case 0xCF:
      CPU_RST(0x08);
   case 0xDF:
      CPU_RST(0x18);
   case 0xEF:
      CPU_RST(0x28);
   case 0xFF:
      CPU_RST(0x38);

   case 0xCB:
   {
      switch (GB_READ_PC())
      {
      case 0x00:
         CPU_RLC(REG_B);
      case 0x01:
         CPU_RLC(REG_C);
      case 0x02:
         CPU_RLC(REG_D);
      case 0x03:
         CPU_RLC(REG_E);
      case 0x04:
         CPU_RLC(REG_H);
      case 0x05:
         CPU_RLC(REG_L);
      case 0x06:
         CPU_RLC_HL();
      case 0x07:
         CPU_RLC(REG_A);

      case 0x08:
         CPU_RRC(REG_B);
      case 0x09:
         CPU_RRC(REG_C);
      case 0x0A:
         CPU_RRC(REG_D);
      case 0x0B:
         CPU_RRC(REG_E);
      case 0x0C:
         CPU_RRC(REG_H);
      case 0x0D:
         CPU_RRC(REG_L);
      case 0x0E:
         CPU_RRC_HL();
      case 0x0F:
         CPU_RRC(REG_A);

      case 0x10:
         CPU_RL(REG_B);
      case 0x11:
         CPU_RL(REG_C);
      case 0x12:
         CPU_RL(REG_D);
      case 0x13:
         CPU_RL(REG_E);
      case 0x14:
         CPU_RL(REG_H);
      case 0x15:
         CPU_RL(REG_L);
      case 0x16:
         CPU_RL_HL();
      case 0x17:
         CPU_RL(REG_A);

      case 0x18:
         CPU_RR(REG_B);
      case 0x19:
         CPU_RR(REG_C);
      case 0x1A:
         CPU_RR(REG_D);
      case 0x1B:
         CPU_RR(REG_E);
      case 0x1C:
         CPU_RR(REG_H);
      case 0x1D:
         CPU_RR(REG_L);
      case 0x1E:
         CPU_RR_HL();
      case 0x1F:
         CPU_RR(REG_A);

      case 0x20:
         CPU_SLA(REG_B);
      case 0x21:
         CPU_SLA(REG_C);
      case 0x22:
         CPU_SLA(REG_D);
      case 0x23:
         CPU_SLA(REG_E);
      case 0x24:
         CPU_SLA(REG_H);
      case 0x25:
         CPU_SLA(REG_L);
      case 0x26:
         CPU_SLA_HL();
      case 0x27:
         CPU_SLA(REG_A);

      case 0x28:
         CPU_SRA(REG_B);
      case 0x29:
         CPU_SRA(REG_C);
      case 0x2A:
         CPU_SRA(REG_D);
      case 0x2B:
         CPU_SRA(REG_E);
      case 0x2C:
         CPU_SRA(REG_H);
      case 0x2D:
         CPU_SRA(REG_L);
      case 0x2E:
         CPU_SRA_HL();
      case 0x2F:
         CPU_SRA(REG_A);

      case 0x30:
         CPU_SWAP(REG_B);
      case 0x31:
         CPU_SWAP(REG_C);
      case 0x32:
         CPU_SWAP(REG_D);
      case 0x33:
         CPU_SWAP(REG_E);
      case 0x34:
         CPU_SWAP(REG_H);
      case 0x35:
         CPU_SWAP(REG_L);
      case 0x36:
         CPU_SWAP_HL();
      case 0x37:
         CPU_SWAP(REG_A);

      case 0x38:
         CPU_SRL(REG_B);
      case 0x39:
         CPU_SRL(REG_C);
      case 0x3A:
         CPU_SRL(REG_D);
      case 0x3B:
         CPU_SRL(REG_E);
      case 0x3C:
         CPU_SRL(REG_H);
      case 0x3D:
         CPU_SRL(REG_L);
      case 0x3E:
         CPU_SRL_HL();
      case 0x3F:
         CPU_SRL(REG_A);

      case 0x40:
         CPU_BIT(0, REG_B);
      case 0x41:
         CPU_BIT(0, REG_C);
      case 0x42:
         CPU_BIT(0, REG_D);
      case 0x43:
         CPU_BIT(0, REG_E);
      case 0x44:
         CPU_BIT(0, REG_H);
      case 0x45:
         CPU_BIT(0, REG_L);
      case 0x46:
         CPU_BIT_HL(0);
      case 0x47:
         CPU_BIT(0, REG_A);

      case 0x48:
         CPU_BIT(1, REG_B);
      case 0x49:
         CPU_BIT(1, REG_C);
      case 0x4A:
         CPU_BIT(1, REG_D);
      case 0x4B:
         CPU_BIT(1, REG_E);
      case 0x4C:
         CPU_BIT(1, REG_H);
      case 0x4D:
         CPU_BIT(1, REG_L);
      case 0x4E:
         CPU_BIT_HL(1);
      case 0x4F:
         CPU_BIT(1, REG_A);

      case 0x50:
         CPU_BIT(2, REG_B);
      case 0x51:
         CPU_BIT(2, REG_C);
      case 0x52:
         CPU_BIT(2, REG_D);
      case 0x53:
         CPU_BIT(2, REG_E);
      case 0x54:
         CPU_BIT(2, REG_H);
      case 0x55:
         CPU_BIT(2, REG_L);
      case 0x56:
         CPU_BIT_HL(2);
      case 0x57:
         CPU_BIT(2, REG_A);

      case 0x58:
         CPU_BIT(3, REG_B);
      case 0x59:
         CPU_BIT(3, REG_C);
      case 0x5A:
         CPU_BIT(3, REG_D);
      case 0x5B:
         CPU_BIT(3, REG_E);
      case 0x5C:
         CPU_BIT(3, REG_H);
      case 0x5D:
         CPU_BIT(3, REG_L);
      case 0x5E:
         CPU_BIT_HL(3);
      case 0x5F:
         CPU_BIT(3, REG_A);

      case 0x60:
         CPU_BIT(4, REG_B);
      case 0x61:
         CPU_BIT(4, REG_C);
      case 0x62:
         CPU_BIT(4, REG_D);
      case 0x63:
         CPU_BIT(4, REG_E);
      case 0x64:
         CPU_BIT(4, REG_H);
      case 0x65:
         CPU_BIT(4, REG_L);
      case 0x66:
         CPU_BIT_HL(4);
      case 0x67:
         CPU_BIT(4, REG_A);

      case 0x68:
         CPU_BIT(5, REG_B);
      case 0x69:
         CPU_BIT(5, REG_C);
      case 0x6A:
         CPU_BIT(5, REG_D);
      case 0x6B:
         CPU_BIT(5, REG_E);
      case 0x6C:
         CPU_BIT(5, REG_H);
      case 0x6D:
         CPU_BIT(5, REG_L);
      case 0x6E:
         CPU_BIT_HL(5);
      case 0x6F:
         CPU_BIT(5, REG_A);

      case 0x70:
         CPU_BIT(6, REG_B);
      case 0x71:
         CPU_BIT(6, REG_C);
      case 0x72:
         CPU_BIT(6, REG_D);
      case 0x73:
         CPU_BIT(6, REG_E);
      case 0x74:
         CPU_BIT(6, REG_H);
      case 0x75:
         CPU_BIT(6, REG_L);
      case 0x76:
         CPU_BIT_HL(6);
      case 0x77:
         CPU_BIT(6, REG_A);

      case 0x78:
         CPU_BIT(7, REG_B);
      case 0x79:
         CPU_BIT(7, REG_C);
      case 0x7A:
         CPU_BIT(7, REG_D);
      case 0x7B:
         CPU_BIT(7, REG_E);
      case 0x7C:
         CPU_BIT(7, REG_H);
      case 0x7D:
         CPU_BIT(7, REG_L);
      case 0x7E:
         CPU_BIT_HL(7);
      case 0x7F:
         CPU_BIT(7, REG_A);

      case 0x80:
         CPU_RES(0, REG_B);
      case 0x81:
         CPU_RES(0, REG_C);
      case 0x82:
         CPU_RES(0, REG_D);
      case 0x83:
         CPU_RES(0, REG_E);
      case 0x84:
         CPU_RES(0, REG_H);
      case 0x85:
         CPU_RES(0, REG_L);
      case 0x86:
         CPU_RES_HL(0);
      case 0x87:
         CPU_RES(0, REG_A);

      case 0x88:
         CPU_RES(1, REG_B);
      case 0x89:
         CPU_RES(1, REG_C);
      case 0x8A:
         CPU_RES(1, REG_D);
      case 0x8B:
         CPU_RES(1, REG_E);
      case 0x8C:
         CPU_RES(1, REG_H);
      case 0x8D:
         CPU_RES(1, REG_L);
      case 0x8E:
         CPU_RES_HL(1);
      case 0x8F:
         CPU_RES(1, REG_A);

      case 0x90:
         CPU_RES(2, REG_B);
      case 0x91:
         CPU_RES(2, REG_C);
      case 0x92:
         CPU_RES(2, REG_D);
      case 0x93:
         CPU_RES(2, REG_E);
      case 0x94:
         CPU_RES(2, REG_H);
      case 0x95:
         CPU_RES(2, REG_L);
      case 0x96:
         CPU_RES_HL(2);
      case 0x97:
         CPU_RES(2, REG_A);

      case 0x98:
         CPU_RES(3, REG_B);
      case 0x99:
         CPU_RES(3, REG_C);
      case 0x9A:
         CPU_RES(3, REG_D);
      case 0x9B:
         CPU_RES(3, REG_E);
      case 0x9C:
         CPU_RES(3, REG_H);
      case 0x9D:
         CPU_RES(3, REG_L);
      case 0x9E:
         CPU_RES_HL(3);
      case 0x9F:
         CPU_RES(3, REG_A);

      case 0xA0:
         CPU_RES(4, REG_B);
      case 0xA1:
         CPU_RES(4, REG_C);
      case 0xA2:
         CPU_RES(4, REG_D);
      case 0xA3:
         CPU_RES(4, REG_E);
      case 0xA4:
         CPU_RES(4, REG_H);
      case 0xA5:
         CPU_RES(4, REG_L);
      case 0xA6:
         CPU_RES_HL(4);
      case 0xA7:
         CPU_RES(4, REG_A);

      case 0xA8:
         CPU_RES(5, REG_B);
      case 0xA9:
         CPU_RES(5, REG_C);
      case 0xAA:
         CPU_RES(5, REG_D);
      case 0xAB:
         CPU_RES(5, REG_E);
      case 0xAC:
         CPU_RES(5, REG_H);
      case 0xAD:
         CPU_RES(5, REG_L);
      case 0xAE:
         CPU_RES_HL(5);
      case 0xAF:
         CPU_RES(5, REG_A);

      case 0xB0:
         CPU_RES(6, REG_B);
      case 0xB1:
         CPU_RES(6, REG_C);
      case 0xB2:
         CPU_RES(6, REG_D);
      case 0xB3:
         CPU_RES(6, REG_E);
      case 0xB4:
         CPU_RES(6, REG_H);
      case 0xB5:
         CPU_RES(6, REG_L);
      case 0xB6:
         CPU_RES_HL(6);
      case 0xB7:
         CPU_RES(6, REG_A);

      case 0xB8:
         CPU_RES(7, REG_B);
      case 0xB9:
         CPU_RES(7, REG_C);
      case 0xBA:
         CPU_RES(7, REG_D);
      case 0xBB:
         CPU_RES(7, REG_E);
      case 0xBC:
         CPU_RES(7, REG_H);
      case 0xBD:
         CPU_RES(7, REG_L);
      case 0xBE:
         CPU_RES_HL(7);
      case 0xBF:
         CPU_RES(7, REG_A);

      case 0xC0:
         CPU_SET(0, REG_B);
      case 0xC1:
         CPU_SET(0, REG_C);
      case 0xC2:
         CPU_SET(0, REG_D);
      case 0xC3:
         CPU_SET(0, REG_E);
      case 0xC4:
         CPU_SET(0, REG_H);
      case 0xC5:
         CPU_SET(0, REG_L);
      case 0xC6:
         CPU_SET_HL(0);
      case 0xC7:
         CPU_SET(0, REG_A);

      case 0xC8:
         CPU_SET(1, REG_B);
      case 0xC9:
         CPU_SET(1, REG_C);
      case 0xCA:
         CPU_SET(1, REG_D);
      case 0xCB:
         CPU_SET(1, REG_E);
      case 0xCC:
         CPU_SET(1, REG_H);
      case 0xCD:
         CPU_SET(1, REG_L);
      case 0xCE:
         CPU_SET_HL(1);
      case 0xCF:
         CPU_SET(1, REG_A);

      case 0xD0:
         CPU_SET(2, REG_B);
      case 0xD1:
         CPU_SET(2, REG_C);
      case 0xD2:
         CPU_SET(2, REG_D);
      case 0xD3:
         CPU_SET(2, REG_E);
      case 0xD4:
         CPU_SET(2, REG_H);
      case 0xD5:
         CPU_SET(2, REG_L);
      case 0xD6:
         CPU_SET_HL(2);
      case 0xD7:
         CPU_SET(2, REG_A);

      case 0xD8:
         CPU_SET(3, REG_B);
      case 0xD9:
         CPU_SET(3, REG_C);
      case 0xDA:
         CPU_SET(3, REG_D);
      case 0xDB:
         CPU_SET(3, REG_E);
      case 0xDC:
         CPU_SET(3, REG_H);
      case 0xDD:
         CPU_SET(3, REG_L);
      case 0xDE:
         CPU_SET_HL(3);
      case 0xDF:
         CPU_SET(3, REG_A);

      case 0xE0:
         CPU_SET(4, REG_B);
      case 0xE1:
         CPU_SET(4, REG_C);
      case 0xE2:
         CPU_SET(4, REG_D);
      case 0xE3:
         CPU_SET(4, REG_E);
      case 0xE4:
         CPU_SET(4, REG_H);
      case 0xE5:
         CPU_SET(4, REG_L);
      case 0xE6:
         CPU_SET_HL(4);
      case 0xE7:
         CPU_SET(4, REG_A);

      case 0xE8:
         CPU_SET(5, REG_B);
      case 0xE9:
         CPU_SET(5, REG_C);
      case 0xEA:
         CPU_SET(5, REG_D);
      case 0xEB:
         CPU_SET(5, REG_E);
      case 0xEC:
         CPU_SET(5, REG_H);
      case 0xED:
         CPU_SET(5, REG_L);
      case 0xEE:
         CPU_SET_HL(5);
      case 0xEF:
         CPU_SET(5, REG_A);

      case 0xF0:
         CPU_SET(6, REG_B);
      case 0xF1:
         CPU_SET(6, REG_C);
      case 0xF2:
         CPU_SET(6, REG_D);
      case 0xF3:
         CPU_SET(6, REG_E);
      case 0xF4:
         CPU_SET(6, REG_H);
      case 0xF5:
         CPU_SET(6, REG_L);
      case 0xF6:
         CPU_SET_HL(6);
      case 0xF7:
         CPU_SET(6, REG_A);

      case 0xF8:
         CPU_SET(7, REG_B);
      case 0xF9:
         CPU_SET(7, REG_C);
      case 0xFA:
         CPU_SET(7, REG_D);
      case 0xFB:
         CPU_SET(7, REG_E);
      case 0xFC:
         CPU_SET(7, REG_H);
      case 0xFD:
         CPU_SET(7, REG_L);
      case 0xFE:
         CPU_SET_HL(7);
      case 0xFF:
         CPU_SET(7, REG_A);

      default:
         gbemu_printf("(0xCB)");
         goto unknown_opcode;

      }
   }

   case 0xD3:
   case 0xE3:
   case 0xE4:
   case 0xF4:
   case 0xDB:
   case 0xEB:
   case 0xEC:
   case 0xFC:
   case 0xDD:
   case 0xED:
   case 0xFD:
invalid_opcode:
      {
         extern retro_environment_t environ_cb;
         retro_sleep(10);
         printf("invalid opcode : 0x%02X\n",GB_READ_U8(CPU.PC - 1));
         fflush(stdout);
//         DEBUG_BREAK();

         if (environ_cb)
            environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
#ifdef PERF_TEST
         extern struct retro_perf_callback perf_cb;
         perf_cb.perf_log();
#endif
         //      return;
         exit(0);
      }
      break;


   default:
unknown_opcode:
      {
         extern retro_environment_t environ_cb;
         retro_sleep(10);
         printf("unknown opcode : 0x%02X\n", GB_READ_U8(CPU.PC - 1));
         fflush(stdout);
//         DEBUG_BREAK();

         if (environ_cb)
            environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
#ifdef PERF_TEST
         extern struct retro_perf_callback perf_cb;
         perf_cb.perf_log();
#endif
         //      return;
         exit(0);
      }
      break;
   }
   
cpu_exit:
   GB.CPU = CPU;
   return;
}
