#ifndef CORE_MSG_H_FILE
#define CORE_MSG_H_FILE

#include <stdint.h>
#include <stdbool.h>
#include "pico/util/queue.h"

#include "lib/mod_data.h"

#define MSG_QUEUE_LENGTH 4  // max messages in queue

#ifdef __cplusplus
extern "C" {
#endif

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
  int audio_pin;
  int sample_freq;
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
  uint32_t frequency;
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

extern queue_t core_msg_queue;

static inline void core_msg_init(void)
{
  queue_init(&core_msg_queue, sizeof(union CORE_MSG), MSG_QUEUE_LENGTH);
}

static inline bool core_msg_can_recv(void) { return queue_is_empty(&core_msg_queue); }
static inline bool core_msg_can_send(void) { return ! queue_is_full(&core_msg_queue); }

static inline void core_msg_send(union CORE_MSG *msg) { return queue_add_blocking(&core_msg_queue, msg); }
static inline void core_msg_recv(union CORE_MSG *msg) { return queue_remove_blocking(&core_msg_queue, msg); }

static inline bool core_msg_try_send(union CORE_MSG *msg) { return queue_try_add(&core_msg_queue, msg); }
static inline bool core_msg_try_recv(union CORE_MSG *msg) { return queue_try_remove(&core_msg_queue, msg); }

static inline void core_msg_set_audio_init(union CORE_MSG *msg, int audio_pin, int sample_freq) {
  msg->msg_header.type        = CORE_MSG_TYPE_AUDIO_INIT;
  msg->audio_init.audio_pin   = audio_pin;
  msg->audio_init.sample_freq = sample_freq;
}

static inline void core_msg_set_audio_play_once(union CORE_MSG *msg, const uint8_t *samples, uint32_t len, uint16_t volume) {
  msg->msg_header.type         = CORE_MSG_TYPE_AUDIO_PLAY_ONCE;
  msg->audio_play_once.samples = samples;
  msg->audio_play_once.len     = len;
  msg->audio_play_once.volume  = volume;
}

static inline void core_msg_set_audio_play_loop(union CORE_MSG *msg, const uint8_t *samples, uint32_t len, uint32_t loop_start, uint16_t volume) {
  msg->msg_header.type            = CORE_MSG_TYPE_AUDIO_PLAY_LOOP;
  msg->audio_play_loop.samples    = samples;
  msg->audio_play_loop.len        = len;
  msg->audio_play_loop.loop_start = loop_start;
  msg->audio_play_loop.volume     = volume;
}

static inline void core_msg_set_music_start(union CORE_MSG *msg, const struct MOD_DATA *music, uint32_t frequency, uint16_t volume, bool loop) {
  msg->msg_header.type       = CORE_MSG_TYPE_MUSIC_START;
  msg->music_start.music     = music;
  msg->music_start.frequency = frequency;
  msg->music_start.volume    = volume;
  msg->music_start.loop      = loop;
}

static inline void core_msg_set_music_stop(union CORE_MSG *msg) {
  msg->msg_header.type = CORE_MSG_TYPE_MUSIC_STOP;
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_MSG_H_FILE */
