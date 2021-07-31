
#include "sfx.h"

#include "core_msg.h"

static int audio_frequency;

void sfx_init(int sound_pin, int sample_freq)
{
  union CORE_MSG msg;
  core_msg_set_audio_init(&msg, sound_pin, sample_freq);
  core_msg_send(&msg);

  audio_frequency = sample_freq;
}

void sfx_play_once(const struct GAME_SFX *sfx, uint16_t volume)
{
  union CORE_MSG msg;
  core_msg_set_audio_play_once(&msg, sfx->samples, sfx->len, volume);
  core_msg_send(&msg);
}

void sfx_play_loop(const struct GAME_SFX *sfx, uint16_t volume)
{
  union CORE_MSG msg;
  core_msg_set_audio_play_loop(&msg, sfx->samples, sfx->len, sfx->loop_start, volume);
  core_msg_send(&msg);
}

void sfx_music_start(const struct MOD_DATA *music, uint16_t volume, bool loop)
{
  union CORE_MSG msg;
  core_msg_set_music_start(&msg, music, audio_frequency, volume, loop);
  core_msg_send(&msg);
}

void sfx_music_stop(void)
{
  union CORE_MSG msg;
  core_msg_set_music_stop(&msg);
  core_msg_send(&msg);
}
