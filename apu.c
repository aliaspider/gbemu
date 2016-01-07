
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
         if (GB.APU.frame_sequencer.length_counter_seq == GBEMU_TIMER_LENGTH_COUNTER_TICK_VAL)
         {
            if (GB.SND_regs.channels.square1.length_enable)
            {
               GB.APU.square1.length_counter.counter++;
               if (GB.APU.square1.length_counter.counter & 0x40)
                  GB.APU.square1.length_counter.ch_enabled = false;
            }
         }
         if (GB.APU.frame_sequencer.sweep_seq == GBEMU_TIMER_SWEEP_TICK_VAL)
         {

            if(GB.APU.square1.sweep.enabled)
            {
               if(GB.APU.square1.sweep.counter)
                  GB.APU.square1.sweep.counter--;
               else
               {
                  GB.APU.square1.sweep.counter = GB.SND_regs.channels.square1.sweep_period;


                  if(GB.SND_regs.channels.square1.sweep_negate)
                     GB.APU.square1.sweep.frequency -= (GB.APU.square1.sweep.frequency >> GB.SND_regs.channels.square1.sweep_shift);
                  else
                  {
                     GB.APU.square1.sweep.frequency += (GB.APU.square1.sweep.frequency >> GB.SND_regs.channels.square1.sweep_shift);
                     if(GB.APU.square1.sweep.frequency & 0x800)
                        GB.APU.square1.length_counter.ch_enabled = false;
                  }

                  GB.SND_regs.channels.square1.frequency = GB.APU.square1.sweep.frequency;

               }

            }

         }
         if (GB.APU.frame_sequencer.envelope_seq == GBEMU_TIMER_ENVELOPE_TICK_VAL)
         {
            if ((GB.APU.square1.envelope.counter) &&
                  !(GB.APU.square1.envelope.counter & 0x8))
            {
               GB.APU.square1.envelope.counter--;
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
      }

      if (!(GB.APU.counter & 0x1F))
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
            GB.APU.square1.pos &=0x7;
            GB.APU.square1.value = (0xCF3C3010 >> ((GB.SND_regs.channels.square1.duty << 3) + GB.APU.square1.pos)) & 1;
         }

      }
      if (!(GB.APU.counter & (GBEMU_AUDIO_DECIMATION_RATE - 1)))
      {
         *GB.APU.write_pos++ = GB.APU.square1.value << 12;
         *GB.APU.write_pos++ = GB.APU.square1.value << 12;
      }

   }

finish:
   GB.APU.cycles = target_cycles;

}