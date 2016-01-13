
#include "gbemu.h"
#include "libretro.h"

extern retro_audio_sample_batch_t audio_batch_cb;

uint16_t gbemu_sound_buffer[0x40000 / GBEMU_AUDIO_DECIMATION_RATE];

void gbemu_apu_run(int target_cycles)
{
   target_cycles *= 4;


   int i;
   for (i = GB.APU.cycles; i < target_cycles ; i++)
   {
      GB.APU.counter++;
      if (!(GB.APU.counter & GBEMU_TIMER_FRAME_SEQUENCER_MASK))
      {
         GB.APU.frame_sequencer.counter++;
         //if ((GB.APU.frame_sequencer.length_counter_seq) == GBEMU_TIMER_LENGTH_COUNTER_TICK_VAL)
         if ((GB.APU.frame_sequencer.counter & 0x1) == GBEMU_TIMER_LENGTH_COUNTER_TICK_VAL)
         {
            if (GB.SND_regs.channels.square1.length_enable)
            {
               GB.SND_regs.channels.square1.length_load++;
               if (!GB.SND_regs.channels.square1.length_load)
                  GB.SND_regs.channels.master.L_square1_enable = 0;
            }
            if (GB.SND_regs.channels.square2.length_enable)
            {
               GB.SND_regs.channels.square2.length_load++;
               if (!GB.SND_regs.channels.square2.length_load)
                  GB.SND_regs.channels.master.L_square2_enable = 0;
            }
            if (GB.SND_regs.channels.wave.length_enable)
            {
               GB.SND_regs.channels.wave.length_load++;
               if (!GB.SND_regs.channels.wave.length_load)
                  GB.SND_regs.channels.master.L_wave_enable = 0;
            }
            if (GB.SND_regs.channels.noise.length_enable)
            {
               GB.SND_regs.channels.noise.length_load++;
               if (!GB.SND_regs.channels.noise.length_load)
                  GB.SND_regs.channels.master.L_noise_enable = 0;
            }
         }

         //         if (GB.APU.frame_sequencer.sweep_seq == GBEMU_TIMER_SWEEP_TICK_VAL)
         if ((GB.APU.frame_sequencer.counter & 0x3) == GBEMU_TIMER_SWEEP_TICK_VAL)
         {

            if (GB.APU.square1.sweep.enabled)
            {
               if (GB.APU.square1.sweep.counter)
                  GB.APU.square1.sweep.counter--;
               else
               {
                  GB.APU.square1.sweep.counter = GB.SND_regs.channels.square1.sweep_period;


                  if (GB.SND_regs.channels.square1.sweep_negate)
                     GB.APU.square1.sweep.frequency -= (GB.APU.square1.sweep.frequency >> GB.SND_regs.channels.square1.sweep_shift);
                  else
                  {
                     GB.APU.square1.sweep.frequency += (GB.APU.square1.sweep.frequency >> GB.SND_regs.channels.square1.sweep_shift);
                     if (GB.APU.square1.sweep.frequency & 0x800)
                        GB.SND_regs.channels.master.L_square1_enable = 0;
                  }

                  GB.SND_regs.channels.square1.frequency = GB.APU.square1.sweep.frequency;

               }

            }

         }
         //if ((GB.APU.frame_sequencer.envelope_seq & 0x7) == GBEMU_TIMER_ENVELOPE_TICK_VAL)
         if ((GB.APU.frame_sequencer.counter & 0x7) == GBEMU_TIMER_ENVELOPE_TICK_VAL)
         {
            if (GB.APU.square1.envelope.counter)
            {
               GB.APU.square1.envelope.counter--;
               if (!GB.APU.square1.envelope.counter)
               {
                  GB.APU.square1.envelope.counter = GB.SND_regs.channels.square1.envelope_period;
                  if (GB.APU.square1.envelope.increment)
                  {
                     if (GB.APU.square1.envelope.volume != 0xF)
                        GB.APU.square1.envelope.volume++;
                  }
                  else
                  {
                     if (GB.APU.square1.envelope.volume)
                        GB.APU.square1.envelope.volume--;
                  }
               }
            }
            if (GB.APU.square2.envelope.counter)
            {
               GB.APU.square2.envelope.counter--;
               if (!GB.APU.square2.envelope.counter)
               {
                  GB.APU.square2.envelope.counter = GB.SND_regs.channels.square2.envelope_period;
                  if (GB.APU.square2.envelope.increment)
                  {
                     if (GB.APU.square2.envelope.volume != 0xF)
                        GB.APU.square2.envelope.volume++;
                  }
                  else
                  {
                     if (GB.APU.square2.envelope.volume)
                        GB.APU.square2.envelope.volume--;
                  }
               }
            }
            if (GB.APU.noise.envelope.counter)
            {
               GB.APU.noise.envelope.counter--;
               if (!GB.APU.noise.envelope.counter)
               {
                  GB.APU.noise.envelope.counter = GB.SND_regs.channels.noise.envelope_period;
                  if (GB.APU.noise.envelope.increment)
                  {
                     if (GB.APU.noise.envelope.volume != 0xF)
                        GB.APU.noise.envelope.volume++;
                  }
                  else
                  {
                     if (GB.APU.noise.envelope.volume)
                        GB.APU.noise.envelope.volume--;
                  }
               }
            }
         }
      }

      if (!(GB.APU.counter & 0x03))
      {
         /*  duty                  --  Pos --
          *  ----   0b000 0b001 0b010 0b011 0b100 0b101 0b110 0b111
          *  0b00     0     0     0     0     1     0     0     0    =  0x10
          *  0b01     0     0     0     0     1     1     0     0    =  0x30
          *  0b10     0     0     1     1     1     1     0     0    =  0x3C
          *  0b11     1     1     1     1     0     0     1     1    =  0xCF
          *
          *
          * (0xCF3C3010 >> ((duty << 3) + pos)) & 1
          *
          */

         GB.APU.square1.counter++;
         if (GB.APU.square1.counter & 0x800)
         {
            GB.APU.square1.counter = GB.SND_regs.channels.square1.frequency;
            GB.APU.square1.pos ++;
            GB.APU.square1.pos &= 0x7;
            GB.APU.square1.value = (0xCF3C3010 >> ((GB.SND_regs.channels.square1.duty << 3) + GB.APU.square1.pos)) & 1;
         }
         GB.APU.square2.counter++;
         if (GB.APU.square2.counter & 0x800)
         {
            GB.APU.square2.counter = GB.SND_regs.channels.square2.frequency;
            GB.APU.square2.pos ++;
            GB.APU.square2.pos &= 0x7;
            GB.APU.square2.value = (0xCF3C3010 >> ((GB.SND_regs.channels.square2.duty << 3) + GB.APU.square2.pos)) & 1;
         }

      }
      if (!(GB.APU.counter & 0x1))
      {
         GB.APU.wave.counter++;
         if (GB.APU.wave.counter & 0x800)
         {
            GB.APU.wave.counter = GB.SND_regs.channels.wave.frequency;
            GB.APU.wave.pos ++;
            GB.APU.wave.pos &= 0x1F;
            GB.APU.wave.value = ((GB.SND_regs.WAVE_TABLE[GB.APU.wave.pos >> 1] >> ((GB.APU.wave.pos & 0x1) * 0x4)) & 0xF)
                                >> ((unsigned)GB.SND_regs.channels.wave.volume_code - 1);
         }

      }

      if (!(GB.APU.counter & 0x3))
      {
         //            GB.APU.noise.down_counter--;
         //         if(!GB.APU.noise.down_counter)
         if (GB.APU.noise.down_counter)
            GB.APU.noise.down_counter--;
         //         else
         {
            if (!GB.SND_regs.channels.noise.divisor_code)
               GB.APU.noise.down_counter = 1;
            else
               GB.APU.noise.down_counter = GB.SND_regs.channels.noise.divisor_code << 1;

            //            GB.APU.noise.down_counter = GB.SND_regs.channels.noise.divisor_code;
            GB.APU.noise.shift_counter--;

            if (!GB.APU.noise.shift_counter)
            {
               GB.APU.noise.shift_counter = 1 << (GB.SND_regs.channels.noise.clock_shift + 1);

               int XOR_val = GB.APU.noise.PRNG ^ (GB.APU.noise.PRNG >> 1);
               if (GB.SND_regs.channels.noise.LFSR_width_mode)
               {
                  XOR_val >>= 0x5;
                  GB.APU.noise.value = ((GB.APU.noise.PRNG >> 7) & 0x1) ^ 0x1;
               }
               else
               {
                  XOR_val >>= 0xD;
                  GB.APU.noise.value = ((GB.APU.noise.PRNG >> 15) & 0x1) ^ 0x1;
               }
               GB.APU.noise.PRNG = (GB.APU.noise.PRNG << 1) | (XOR_val & 0x1);
            }

         }
      }
      static int l = 0;
      static int r = 0;
      if(GB.SND_regs.channels.master.L_square1_enable)
         l += (GB.APU.square1.value * GB.APU.square1.envelope.volume);
//      l += GB.APU.square1.value;
      if(GB.SND_regs.channels.master.L_square2_enable)
         l += (GB.APU.square2.value * GB.APU.square2.envelope.volume);
//      l += GB.APU.square2.value;
         if(GB.SND_regs.channels.master.L_wave_enable)
           l += (GB.APU.wave.value );

      if (GB.SND_regs.channels.master.L_noise_enable)
         l += (GB.APU.noise.value * GB.APU.noise.envelope.volume);
      //         l += GB.APU.noise.value << 3;

      if (!(GB.APU.counter & (GBEMU_AUDIO_DECIMATION_RATE - 1)))
      {

         l <<= 9;
         l -= (1 << 8);
         l /= GBEMU_AUDIO_DECIMATION_RATE;
         r = l;
         *GB.APU.write_pos++ = l;
         *GB.APU.write_pos++ = r;

         l = 0;
         r = 0;
      }

   }

finish:
   GB.APU.cycles = target_cycles;

}
