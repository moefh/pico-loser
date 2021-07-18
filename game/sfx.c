
#include "sfx.h"

#include "core_msg.h"

void sfx_init(int sound_pin)
{
  union CORE_MSG msg;
  core_msg_set_audio_init(&msg, sound_pin);
  core_msg_send(&msg);
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
