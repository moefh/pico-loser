
#include "pico/multicore.h"

#include "core_msg.h"

#define MSG_QUEUE_LENGTH 4  // max messages in queue

static queue_t core_msg_queue;

void core_msg_init(void)
{
  queue_init(&core_msg_queue, sizeof(union CORE_MSG), MSG_QUEUE_LENGTH);
}

bool core_msg_can_recv(void)
{
  return queue_is_empty(&core_msg_queue);
}

bool core_msg_can_send(void)
{
  return ! queue_is_full(&core_msg_queue);
}

void core_msg_send(union CORE_MSG *msg)
{
  return queue_add_blocking(&core_msg_queue, msg);
}

void core_msg_recv(union CORE_MSG *msg)
{
  return queue_remove_blocking(&core_msg_queue, msg);
}

bool core_msg_try_send(union CORE_MSG *msg)
{
  return queue_try_add(&core_msg_queue, msg);
}

bool core_msg_try_recv(union CORE_MSG *msg)
{
  return queue_try_remove(&core_msg_queue, msg);
}

void msg_audio_init(int sound_pin, int sample_freq)
{
  union CORE_MSG msg;
  msg.msg_header.type        = CORE_MSG_TYPE_AUDIO_INIT;
  msg.audio_init.sound_pin   = sound_pin;
  msg.audio_init.sample_freq = sample_freq;
  core_msg_send(&msg);
}

void msg_audio_play_once(const struct GAME_SFX *sfx, uint16_t volume)
{
  union CORE_MSG msg;
  msg.msg_header.type         = CORE_MSG_TYPE_AUDIO_PLAY_ONCE;
  msg.audio_play_once.samples = sfx->samples;
  msg.audio_play_once.len     = sfx->len;
  msg.audio_play_once.volume  = volume;
  core_msg_send(&msg);
}

void msg_audio_play_loop(const struct GAME_SFX *sfx, uint16_t volume)
{
  union CORE_MSG msg;
  msg.msg_header.type            = CORE_MSG_TYPE_AUDIO_PLAY_LOOP;
  msg.audio_play_loop.samples    = sfx->samples;
  msg.audio_play_loop.len        = sfx->len;
  msg.audio_play_loop.loop_start = sfx->loop_start;
  msg.audio_play_loop.volume     = volume;
  core_msg_send(&msg);
}

void msg_music_start(const struct MOD_DATA *music, int sample_freq, uint16_t volume, bool loop)
{
  union CORE_MSG msg;
  msg.msg_header.type         = CORE_MSG_TYPE_MUSIC_START;
  msg.music_start.music       = music;
  msg.music_start.sample_freq = sample_freq;
  msg.music_start.volume      = volume;
  msg.music_start.loop        = loop;
  core_msg_send(&msg);
}

void msg_music_stop(void)
{
  union CORE_MSG msg;
  msg.msg_header.type = CORE_MSG_TYPE_MUSIC_STOP;
  core_msg_send(&msg);
}
