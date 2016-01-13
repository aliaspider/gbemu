
#include <stdlib.h>
#include <stdio.h>

#include "libretro.h"
#include "gbemu.h"

void perf_register_callback(struct retro_perf_counter* counter)
{

}

void perf_start_callback(struct retro_perf_counter* counter)
{

}

void perf_stop_callback(struct retro_perf_counter* counter)
{

}
void perf_log_callback(void)
{

}

bool environment_callback(unsigned cmd, void* data)
{
   switch (cmd)
   {
   case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
   {
      struct retro_perf_callback* perf_callbacks = (struct retro_perf_callback*)data;
      perf_callbacks->perf_register = perf_register_callback;
      perf_callbacks->perf_start = perf_start_callback;
      perf_callbacks->perf_stop = perf_stop_callback;
      perf_callbacks->perf_log = perf_log_callback;
      return true;
   }
   }

   return false;
}

void video_callback(const void* data, unsigned width,
                    unsigned height, size_t pitch)
{
   //   printf("video_cb(0x%X, %i, %i, %i);\n", data, width, height, pitch);
   return;
}

size_t audio_sample_batch_callback(const int16_t* data,
                                   size_t frames)
{
   //   printf("audio_batch_cb(0x%X, %i);\n", data, frames);
   return frames;
}

void retro_input_poll_callback(void)
{

}

int16_t input_state_callback(unsigned port, unsigned device,
                             unsigned index, unsigned id)
{
   return 0;
}

int main(int argc, const char** argv)
{
   if ((argc < 2) || !argv[1] || !argv[1][0])
   {
      printf("usage %s <path-to-test-rom>\n", argv[0]);
      return 1;
   }

   bool verbose_on = true;

   if (!strcmp(argv[1], "--silent"))
      verbose_on = false;

   if (argc == 3 && !strcmp(argv[2], "--silent"))
      verbose_on = false;

   retro_set_environment(environment_callback);
   retro_set_video_refresh(video_callback);
   retro_set_audio_sample_batch(audio_sample_batch_callback);
   retro_set_input_poll(retro_input_poll_callback);
   retro_set_input_state(input_state_callback);

   retro_init();

   struct retro_game_info game_info;

   game_info.path = argv[1];
   FILE* fp = fopen(game_info.path, "rb");

   if (!fp)
   {
      printf("file not found : %s\n", argv[1]);
      return 1;
   }

   fseek(fp, 0, SEEK_END);
   game_info.size = ftell(fp);
   game_info.data = malloc(game_info.size);

   fseek(fp, 0, SEEK_SET);
   fread((void*)game_info.data, 1, game_info.size, fp);
   fclose(fp);

   retro_load_game(&game_info);
   free((void*)game_info.data);
   game_info.data = NULL;

   int frames, i;
   struct __attribute__((packed)) run_info_t
   {
      uint8_t status;
      uint32_t signature;
      char output[512];
   }*run_info = (struct run_info_t*)((uint8_t*)retro_get_memory_data(RETRO_MEMORY_SAVE_RAM) + 0x2000);

   for (frames = 0; frames < 6000; frames++)
   {
      //      printf("frame : %i\n", frames);
      retro_run();

      if (run_info->signature == 0x3061b0de)
      {
         /* type 1*/
         if (run_info->status != 0x80)
         {
            if (verbose_on)
            {
               printf("%s\n", run_info->output);
               printf("frames : %i\nexit code : 0x%02X\n%s\n", frames, run_info->status,
                      run_info->status ? "FAILURE !!" : "SUCCESS !!");
            }

            return (run_info->status);
         }

      }
      else
      {
         GB.serial_port.buffer[GB.serial_port.write_index] = 0;

         //         printf("%i : %s\n", GB.serial_port.write_index, GB.serial_port.buffer);
         if (GB.serial_port.write_index > 6)
         {
            //            printf("\n%s\n", &GB.serial_port.buffer[GB.serial_port.write_index - 7]);
            if (!memcmp(&GB.serial_port.buffer[GB.serial_port.write_index - 7], "Passed", 6))
            {
               if (verbose_on)
               {
                  printf("%s\n", GB.serial_port.buffer);
                  printf("frames : %i\nSUCCESS !!\n", frames);
               }

               return 0;
            }
            else if (!memcmp(&GB.serial_port.buffer[GB.serial_port.write_index - 7], "Failed", 6))
            {
               if (verbose_on)
               {
                  printf("%s\n", GB.serial_port.buffer);
                  printf("frames : %i\nFAILURE !!\n", frames);
               }

               return 1;
            }
         }
      }

   }

   if (verbose_on)
   {
      if (run_info->signature == 0x3061b0de)
         printf("%s\n", run_info->output);
      else if (GB.serial_port.write_index > 6)
      {
         GB.serial_port.buffer[GB.serial_port.write_index] = 0;
         printf("%s\n", GB.serial_port.buffer);
      }

   }

   printf("frames : %i (timeout)\nFAILURE !!\n", frames);

   retro_deinit();

   return 1;
}
