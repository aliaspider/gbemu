
#include <stdlib.h>
#include <stdio.h>

#include "libretro.h"

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
   switch(cmd)
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
   printf("video_cb(0x%X, %i, %i, %i);\n", data, width, height, pitch);
   return;
}

size_t audio_sample_batch_callback(const int16_t* data,
      size_t frames)
{
   printf("audio_batch_cb(0x%X, %i);\n", data, frames);
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

int main (int argc, const char** argv)
{
   if (argc != 2)
   {
      printf("usage %s <path-to-test-rom>\n", argv[0]);
      return 1;
   }




   retro_set_environment(environment_callback);
   retro_set_video_refresh(video_callback);
   retro_set_audio_sample_batch(audio_sample_batch_callback);
   retro_set_input_poll(retro_input_poll_callback);
   retro_set_input_state(input_state_callback);

   retro_init();

   struct retro_game_info game_info;

   game_info.path = argv[1];
   FILE* fp = fopen(game_info.path, "rb");
   fseek(fp, 0, SEEK_END);
   game_info.size = ftell(fp);
   game_info.data = malloc(game_info.size);

   fseek(fp, 0, SEEK_SET);
   fread((void*)game_info.data, 1, game_info.size, fp);
   fclose(fp);

   retro_load_game(&game_info);
   free((void*)game_info.data);
   game_info.data = NULL;

   int frames;
   for(frames = 0; frames < 600; frames++)
   {
      printf("frame : %i\n", frames);
      retro_run();

   }


   retro_deinit();

   return 0;
}
