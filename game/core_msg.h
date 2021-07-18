#ifndef CORE_MSG_H_FILE
#define CORE_MSG_H_FILE

#include <stdint.h>
#include <stdbool.h>
#include "pico/util/queue.h"

#define MSG_QUEUE_LENGTH 4  // max messages in queue

#ifdef __cplusplus
extern "C" {
#endif

// message types
enum CORE_MSG_TYPE {
  CORE_MSG_TYPE_AUDIO_INIT,
  CORE_MSG_TYPE_AUDIO_PLAY_ONCE,
  CORE_MSG_TYPE_AUDIO_PLAY_LOOP,
};

struct CORE_MSG_HEADER {
  enum CORE_MSG_TYPE type;
};

struct CORE_MSG_AUDIO_INIT {
  struct CORE_MSG_HEADER msg_header;
  int audio_pin;
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

union CORE_MSG {
  struct CORE_MSG_HEADER msg_header;
  struct CORE_MSG_AUDIO_INIT audio_init;
  struct CORE_MSG_AUDIO_PLAY_ONCE audio_play_once;
  struct CORE_MSG_AUDIO_PLAY_LOOP audio_play_loop;
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


static inline void core_msg_set_audio_init(union CORE_MSG *msg, int audio_pin) {
  msg->msg_header.type      = CORE_MSG_TYPE_AUDIO_INIT;
  msg->audio_init.audio_pin = audio_pin;
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

#ifdef __cplusplus
}
#endif

#endif /* CORE_MSG_H_FILE */
