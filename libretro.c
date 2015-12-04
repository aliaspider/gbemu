
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "libretro.h"
#include "retro_miscellaneous.h"
#include "gbemu.h"

static retro_log_printf_t log_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_input_poll_t poll_cb = NULL;
static retro_input_state_t input_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;
struct retro_perf_callback perf_cb;

#ifdef PERF_TEST
#define RETRO_PERFORMANCE_INIT(name) \
   retro_perf_tick_t current_ticks;\
   static struct retro_perf_counter name = {#name};\
   if (!name.registered) perf_cb.perf_register(&(name));\
   current_ticks = name.total

#define RETRO_PERFORMANCE_START(name) perf_cb.perf_start(&(name))
#define RETRO_PERFORMANCE_STOP(name) \
   perf_cb.perf_stop(&(name));\
   current_ticks = name.total - current_ticks;
#else
#define RETRO_PERFORMANCE_INIT(name)
#define RETRO_PERFORMANCE_START(name)
#define RETRO_PERFORMANCE_STOP(name)
#endif

void gbemu_wait_for_input(void)
{
   if(!poll_cb || !input_cb)
      return;

   printf("press START\n");
   fflush(stdout);

   do
   {
      retro_sleep(50);
      poll_cb();
      if(input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))
         exit(0);
   }
   while(!input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START));
}

void gbemu_check_exit_request(void)
{
   if(!poll_cb || !input_cb)
      return;

   poll_cb();
   if(input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))
//      environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
      exit(0);
}


void retro_set_environment(retro_environment_t cb)
{
   struct retro_log_callback log;

   environ_cb = cb;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb);
}

void retro_init(void)
{
   struct retro_log_callback log;
   enum retro_pixel_format rgb565;
   static const struct retro_variable vars[] =
   {
      { "gbemu_dummy", "dummy; disabled|enabled" },
      { NULL, NULL },
   };

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   rgb565 = RETRO_PIXEL_FORMAT_RGB565;
   if (environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb565) && log_cb)
      log_cb(RETRO_LOG_INFO,
             "Frontend supports RGB565 - will use that instead of XRGB1555.\n");

   environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

void retro_deinit(void)
{
#ifdef PERF_TEST
   perf_cb.perf_log();
#endif
}

static void check_variables(void)
{
   struct retro_variable var;

   var.key = "gbemu_dummy";
   var.value = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
      (void)strcmp(var.value, "enabled");
}


void retro_get_system_info(struct retro_system_info* info)
{
   info->need_fullpath = false;
   info->valid_extensions = "gb";
   info->library_version = "v0.01";
   info->library_name = "GBEMU";
   info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
   info->geometry.base_width = 160;
   info->geometry.base_height = 144;
   info->geometry.max_width = 160;
   info->geometry.max_height = 144;
   info->geometry.aspect_ratio = 0.0;
   info->timing.fps = 60.0;
   info->timing.sample_rate = 32768;
}


static void init_descriptors(void)
{
   struct retro_input_descriptor desc[] =
   {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },

      { 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);
}

void retro_reset(void)
{
}

void retro_run(void)
{
   bool updated = false;
   static int frames = 0;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();

   poll_cb();

   RETRO_PERFORMANCE_INIT(gbemu_main_func);
   RETRO_PERFORMANCE_START(gbemu_main_func);
   gbemu_run();
   RETRO_PERFORMANCE_STOP(gbemu_main_func);

   video_cb(NULL, 160, 144, 320);
//   DEBUG_HOLD();
//   printf("frame : %i\n", frames++);
   fflush(stdout);
}

bool retro_load_game(const struct retro_game_info* game)
{
   init_descriptors();
   check_variables();

   uint8_t bios_data[256];
   char bios_path[PATH_MAX_LENGTH];
   char* system_dir;
   if(environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
      strncpy(bios_path, system_dir, sizeof(bios_path));
   else if (game->path)
      strncpy(bios_path, game->path, sizeof(bios_path));
   else
      strncpy(bios_path, ".", sizeof(bios_path));
#if defined(_WIN32)
   char slash = '\\';
#else
   char slash = '/';
#endif
   char* tmp = strrchr(bios_path, slash);
   if(!tmp)
      tmp = strrchr(bios_path, '\0');
   *tmp++ = slash;
   strcpy(tmp, "gbbios.gb");

   retro_sleep(10);
   fflush(stdout);
   printf("romd info\n");
   printf("path : %s\n", game->path);
   printf("size : %u\n", game->size);
   fflush(stdout);

   printf("loading bios from : %s\n", bios_path);
   FILE* fp = fopen(bios_path, "rb");
   if(fp)
   {
      fread(bios_data, 1, sizeof(bios_data), fp);
      fclose(fp);
   }
   else
      printf("bios not found !\n");

   gbemu_load_game(game->data, game->size, fp? bios_data: NULL);

   return true;
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_cb = cb;
}

void retro_set_controller_port_device(unsigned port, unsigned device) {}

unsigned retro_api_version()
{
   return RETRO_API_VERSION;
}


bool retro_load_game_special(unsigned game_type,
                             const struct retro_game_info* info, size_t num_info)
{
   return false;
}
void retro_unload_game(void)
{
}

void* retro_get_memory_data(unsigned id)
{
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   return 0;
}
size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void* data, size_t size)
{
   return false;
}
bool retro_unserialize(const void* data, size_t size)
{
   return false;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char* code)
{
}
