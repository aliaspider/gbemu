#ifndef APU_H
#define APU_H

#include <stdint.h>

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
            unsigned sweep_period : 3;
            unsigned negate : 1;
            unsigned shift : 3;
            unsigned : 1;

            unsigned length_load : 6;
            unsigned duty : 2;

            unsigned period : 3;
            unsigned envelope_add_mode : 1;
            unsigned starting_volume : 4;

            unsigned frequency : 11;
            unsigned :3;
            unsigned length_enable :1;
            unsigned trigger :1;
         } square1;
         struct __attribute__((packed))
         {
            unsigned : 8;

            unsigned length_load : 6;
            unsigned duty : 2;

            unsigned period : 3;
            unsigned envelope_add_mode : 1;
            unsigned starting_volume : 4;

            unsigned frequency : 11;
            unsigned :3;
            unsigned length_enable :1;
            unsigned trigger :1;
         } square2;
         struct __attribute__((packed))
         {
            unsigned : 7;
            unsigned DAC_power : 1;

            unsigned length_load : 8;

            unsigned : 5;
            unsigned volume_code : 2;
            unsigned : 1;

            unsigned frequency : 11;
            unsigned :3;
            unsigned length_enable :1;
            unsigned trigger :1;
         } wave;
         struct __attribute__((packed))
         {
            unsigned : 8;

            unsigned length_load : 6;
            unsigned : 2;

            unsigned period : 3;
            unsigned envelope_add_mode : 1;
            unsigned starting_volume : 4;

            unsigned divisor_code : 3;
            unsigned LFSR_width_mode :1;
            unsigned clock_shift :4;
            unsigned :6;
            unsigned length_enable :1;
            unsigned trigger :1;
         } noise;

         struct __attribute__((packed))
         {
            unsigned R_vol :3;
            unsigned R_vin_enable :1;
            unsigned L_vol :3;
            unsigned L_vin_enable :1;

            unsigned R_square1_enable :1;
            unsigned R_square2_enable :1;
            unsigned R_wave_enable :1;
            unsigned R_noise_enable :1;
            unsigned L_square1_enable :1;
            unsigned L_square2_enable :1;
            unsigned L_wave_enable :1;
            unsigned L_noise_enable :1;

            unsigned square1_length_status :1;
            unsigned square2_length_status :1;
            unsigned wave_length_status :1;
            unsigned noise_length_status :1;
            unsigned :3;
            unsigned power :1;
         }master;
      }channels;
   };


}
gbemu_sound_regs_t;


#endif // APU_H
