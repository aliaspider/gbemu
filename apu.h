#ifndef APU_H
#define APU_H

#include <stdint.h>

#define GBEMU_AUDIO_DECIMATION_RATE 128

typedef struct __attribute__((packed))
{
   union
   {
      struct __attribute__((packed))
      {
         uint8_t NR10;
         uint8_t NR11;
         uint8_t NR12;
         uint8_t NR13;
         uint8_t NR14;
         unsigned : 8;
         uint8_t NR21;
         uint8_t NR22;
         uint8_t NR23;
         uint8_t NR24;
         uint8_t NR30;
         uint8_t NR31;
         uint8_t NR32;
         uint8_t NR33;
         uint8_t NR34;
         unsigned : 8;
         uint8_t NR41;
         uint8_t NR42;
         uint8_t NR43;
         uint8_t NR44;
         uint8_t NR50;
         uint8_t NR51;
         uint8_t NR52;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         unsigned : 8;
         uint8_t WAVE_TABLE[0x10];
      };
      struct __attribute__((packed))
      {
         struct __attribute__((packed))
         {
            unsigned sweep_shift : 3;
            unsigned sweep_negate : 1;
            unsigned sweep_period : 3;
            unsigned : 1;

            unsigned length_load : 6;
            unsigned duty : 2;

            unsigned envelope_period : 3;
            unsigned envelope_add_mode : 1;
            unsigned envelope_starting_volume : 4;

            unsigned frequency : 11;
            unsigned : 3;
            unsigned length_enable : 1;
            unsigned trigger : 1;
         }
         square1;
         struct __attribute__((packed))
         {
            unsigned : 8;

            unsigned length_load : 6;
            unsigned duty : 2;

            unsigned envelope_period : 3;
            unsigned envelope_add_mode : 1;
            unsigned envelope_starting_volume : 4;

            unsigned frequency : 11;
            unsigned : 3;
            unsigned length_enable : 1;
            unsigned trigger : 1;
         }
         square2;
         struct __attribute__((packed))
         {
            unsigned : 7;
            unsigned DAC_power : 1;

            unsigned length_load : 8;

            unsigned : 5;
            unsigned volume_code : 2;
            unsigned : 1;

            unsigned frequency : 11;
            unsigned : 3;
            unsigned length_enable : 1;
            unsigned trigger : 1;
         }
         wave;
         struct __attribute__((packed))
         {
            unsigned : 8;

            unsigned length_load : 6;
            unsigned : 2;

            unsigned envelope_period : 3;
            unsigned envelope_add_mode : 1;
            unsigned envelope_starting_volume : 4;

            unsigned divisor_code : 3;
            unsigned LFSR_width_mode : 1;
            unsigned clock_shift : 4;
            unsigned : 6;
            unsigned length_enable : 1;
            unsigned trigger : 1;
         }
         noise;

         struct __attribute__((packed))
         {
            unsigned R_vol : 3;
            unsigned R_vin_enable : 1;
            unsigned L_vol : 3;
            unsigned L_vin_enable : 1;

            unsigned R_square1_enable : 1;
            unsigned R_square2_enable : 1;
            unsigned R_wave_enable : 1;
            unsigned R_noise_enable : 1;
            unsigned L_square1_enable : 1;
            unsigned L_square2_enable : 1;
            unsigned L_wave_enable : 1;
            unsigned L_noise_enable : 1;

            unsigned square1_status_flag : 1;
            unsigned square2_status_flag : 1;
            unsigned wave_status_flag : 1;
            unsigned noise_status_flag : 1;
            unsigned : 3;
            unsigned power_ctrl : 1;
         }
         master;
      } channels;
   };


}
gbemu_sound_regs_t;

#define GBEMU_TIMER_FRAME_SEQUENCER_PERIOD   (1 << 13)
#define GBEMU_TIMER_FRAME_SEQUENCER_MASK     (GBEMU_TIMER_FRAME_SEQUENCER_PERIOD >> 1)

#define GBEMU_TIMER_LENGTH_COUNTER_TICK_VAL   (0x0)
#define GBEMU_TIMER_SWEEP_TICK_VAL            (0x10)
#define GBEMU_TIMER_ENVELOPE_TICK_VAL         (0x111)

static const int8_t gbemu_apu_square_wave_duty_cycles[4][8] =
{
   {0, 0, 0, 0, 0, 0, 0, 1},
   {1, 0, 0, 0, 0, 0, 0, 1},
   {1, 0, 0, 0, 0, 1, 1, 1},
   {0, 1, 1, 1, 1, 1, 1, 0}
};

typedef struct
{
   struct
   {
      union
      {
         struct
         {
            unsigned length_counter_seq : 1;
         };
         struct
         {
            unsigned envelope_seq : 3;
         };
         struct
         {
            unsigned sweep_seq : 2;
         };
         int counter;
      };

   } frame_sequencer;
   struct
   {
      struct
      {
         int counter;
         int period;
      } timer;
      struct
      {
         int counter;
         bool ch_enabled;
      } length_counter;
      struct
      {
         int counter;
         int volume;
         bool increment;
      } envelope;
      struct
      {
         int counter;
         bool enabled;
         int frequency;
      } sweep;
      int counter;
      int pos;
      int value;
   } square1;
   struct
   {
      struct
      {
         int counter;
         int period;
      } timer;
      struct
      {
         int counter;
         bool ch_enabled;
      } length_counter;
      struct
      {
         int counter;
         int volume;
         bool increment;
      } envelope;
      struct
      {
         int counter;
         bool enabled;
         int frequency;
      } sweep;
      int counter;
      int pos;
      int value;
   } square2;
   struct
   {
      struct
      {
         int counter;
         int period;
      } timer;
      struct
      {
         int counter;
         bool ch_enabled;
      } length_counter;
      struct
      {
         int counter;
         int volume;
         bool increment;
      } envelope;
      struct
      {
         int counter;
         bool enabled;
         int frequency;
      } sweep;
      int counter;
      int pos;
      int value;
   } wave;
   struct
   {
      struct
      {
         int counter;
         int period;
      } timer;
      struct
      {
         int counter;
         bool ch_enabled;
      } length_counter;
      struct
      {
         int counter;
         int volume;
         bool increment;
      } envelope;
      struct
      {
         int counter;
         bool enabled;
         int frequency;
      } sweep;
      int counter;
      int pos;
      int value;
   } noise;


   int counter;
   int cycles;
   uint16_t* write_pos;
} gbemu_apu_t;

extern uint16_t gbemu_sound_buffer[0x40000 / GBEMU_AUDIO_DECIMATION_RATE];
void gbemu_apu_run(int target_cycles);


#endif // APU_H
