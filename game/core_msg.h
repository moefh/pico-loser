#ifndef CORE_MSG_H_FILE
#define CORE_MSG_H_FILE

#include <stdint.h>
#include <stdbool.h>
#include "pico/util/queue.h"

#include "lib/mod_data.h"
#include "game_data.h"

// message types
enum CORE_MSG_TYPE {
  CORE_MSG_TYPE_AUDIO_INIT,
  CORE_MSG_TYPE_AUDIO_PLAY_ONCE,
  CORE_MSG_TYPE_AUDIO_PLAY_LOOP,
  CORE_MSG_TYPE_MUSIC_START,
  CORE_MSG_TYPE_MUSIC_STOP,
};

struct CORE_MSG_HEADER {
  enum CORE_MSG_TYPE type;
};

struct CORE_MSG_AUDIO_INIT {
  struct CORE_MSG_HEADER msg_header;
  uint32_t sound_pin;
  uint32_t sample_freq;
};

struct CORE_MSG_AUDIO_PLAY_ONCE {
  struct CORE_MSG_HEADER msg_header;
  const uint8_t *samples;
  uint32_t len;
  uint16_t volume;
};

struct CORE_MSG_AUDIO_PLAY_LOOP {
  struct CORE_MSG_HEADER msg_header;
  const uint8_t *samples;
  uint32_t len;
  uint32_t loop_start;
  uint16_t volume;
};

struct CORE_MSG_MUSIC_START {
  struct CORE_MSG_HEADER msg_header;
  const struct MOD_DATA *music;
  uint32_t sample_freq;
  uint16_t volume; // 8.8 fixed point
  bool loop;
};

struct CORE_MSG_MUSIC_STOP {
  struct CORE_MSG_HEADER msg_header;
};

union CORE_MSG {
  struct CORE_MSG_HEADER msg_header;
  struct CORE_MSG_AUDIO_INIT audio_init;
  struct CORE_MSG_AUDIO_PLAY_ONCE audio_play_once;
  struct CORE_MSG_AUDIO_PLAY_LOOP audio_play_loop;
  struct CORE_MSG_MUSIC_START music_start;
  struct CORE_MSG_MUSIC_STOP music_stop;
};

#ifdef __cplusplus
extern "C" {
#endif

void core_msg_init(void);
bool core_msg_can_recv(void);
bool core_msg_can_send(void);
void core_msg_send(union CORE_MSG *msg);
void core_msg_recv(union CORE_MSG *msg);
bool core_msg_try_send(union CORE_MSG *msg);
bool core_msg_try_recv(union CORE_MSG *msg);

void msg_audio_init(int sound_pin, int sample_freq);
void msg_audio_play_once(const struct GAME_SFX *sfx, uint16_t volume);
void msg_audio_play_loop(const struct GAME_SFX *sfx, uint16_t volume);
void msg_music_start(const struct MOD_DATA *music, int sample_freq, uint16_t volume, bool loop);
void msg_music_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* CORE_MSG_H_FILE */
