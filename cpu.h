#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   union
   {
      struct
      {
         union
         {
            struct
            {
               unsigned FL :4;
               unsigned FC :1;
               unsigned FH :1;
               unsigned FN :1;
               unsigned FZ :1;
            };
            uint8_t F;
         };
         uint8_t A;
      };
      uint16_t AF;
   };
   union
   {
      struct
      {
         uint8_t C;
         uint8_t B;
      };
      uint16_t BC;
   };
   union
   {
      struct
      {
         uint8_t E;
         uint8_t D;
      };
      uint16_t DE;
   };
   union
   {
      struct
      {
         uint8_t L;
         uint8_t H;
      };
      uint16_t HL;
   };
   uint16_t SP;
   uint16_t PC;
   int cycles;
   bool interrupts_enabled;
}gbemu_cpu_t;

void gbemu_cpu_run(int cycles);

#endif // CPU_H
