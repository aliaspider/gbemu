
#include "gbemu.h"
#include "libretro.h"

extern retro_audio_sample_batch_t audio_batch_cb;

uint16_t gbemu_sound_buffer[0x800];

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
               {
                  GB.APU.square1.length_counter.ch_enabled = false;
               }
            }
         }
         if (GB.APU.frame_sequencer.sweep_seq == GBEMU_TIMER_SWEEP_TICK_VAL)
         {

         }
         if (GB.APU.frame_sequencer.envelope_seq == GBEMU_TIMER_ENVELOPE_TICK_VAL)
         {

         }

      }

      if (!(GB.APU.counter & 0x1F))
      {
         GB.APU.square1.counter++;
         if (GB.APU.square1.counter & 0x800)
            GB.APU.square1.counter = GB.SND_regs.channels.square1.frequency;

      }
   }

finish:
   GB.APU.cycles = target_cycles;

}
