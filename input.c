

#include "gbemu.h"
#include "libretro.h"

void gbemu_update_input(void)
{
   extern retro_input_state_t input_cb;

   gbemu_input_state_t input_new;

   input_new.right = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
   input_new.left = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
   input_new.up = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
   input_new.down = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);

   input_new.A = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
   input_new.B = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
   input_new.select = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
   input_new.start = !input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);


   if((!GB.JOYP.dpad_select && (input_new.dpad != GB.input.dpad)) ||
      (!GB.JOYP.buttons_select && (input_new.buttons != GB.input.buttons)))
      GB.IF.joypad = 1;

   GB.input = input_new;

}
