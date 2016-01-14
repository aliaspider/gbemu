#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef union __attribute((packed))
{
   struct __attribute((packed))
   {
      unsigned A       : 1;
      unsigned B       : 1;
      unsigned select  : 1;
      unsigned start   : 1;
      unsigned         : 4;
   };
   struct __attribute((packed))
   {
      unsigned right   : 1;
      unsigned left    : 1;
      unsigned up      : 1;
      unsigned down    : 1;
      unsigned         : 4;
   };
   struct __attribute((packed))
   {
      unsigned                : 4;
      unsigned dpad_select    : 1;
      unsigned buttons_select : 1;
   };
   uint8_t val;
} gbemu_input_register_t;

typedef struct
{
   union __attribute((packed))
   {
      struct __attribute((packed))
      {
         unsigned A       : 1;
         unsigned B       : 1;
         unsigned select  : 1;
         unsigned start   : 1;
         unsigned         : 4;
      };
      uint8_t buttons;
   };
   union __attribute((packed))
   {
      struct __attribute((packed))
      {
         unsigned right   : 1;
         unsigned left    : 1;
         unsigned up      : 1;
         unsigned down    : 1;
         unsigned         : 4;
      };
      uint8_t dpad;
   };
} gbemu_input_state_t;

void gbemu_update_input(void);

#endif // INPUT_H
