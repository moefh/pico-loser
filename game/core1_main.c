#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "core_msg.h"
#include "lib/audio.h"
#include "lib/audio_mixer.h"
#include "lib/mod_play.h"

static void mod_play_callback(uint8_t *samples, uint32_t len)
{
  if (mod_play_step(samples, len) != 0) {
    audio_mixer_set_callback(NULL, 0);
  }
}

static void process_message(union CORE_MSG *msg)
{
  //printf("processing message of type %d\n", msg->msg_header.type);
  switch (msg->msg_header.type) {
  case CORE_MSG_TYPE_AUDIO_INIT:
    {
      struct CORE_MSG_AUDIO_INIT *m = &msg->audio_init;
      audio_init(m->sound_pin, m->sample_freq);
      audio_mixer_init();
    }
    return;

  case CORE_MSG_TYPE_AUDIO_PLAY_ONCE:
    {
      struct CORE_MSG_AUDIO_PLAY_ONCE *m = &msg->audio_play_once;
      int source_id = audio_mixer_once(m->samples, m->len);
      if (source_id >= 0) {
        audio_mixer_set_source_volume(source_id, m->volume);
      }
    }
    return;

  case CORE_MSG_TYPE_AUDIO_PLAY_LOOP:
    {
      struct CORE_MSG_AUDIO_PLAY_LOOP *m = &msg->audio_play_loop;
      int source_id = audio_mixer_loop(m->samples, m->len, m->loop_start);
      if (source_id >= 0) {
        audio_mixer_set_source_volume(source_id, m->volume);
      }
    }
    return;

  case CORE_MSG_TYPE_MUSIC_START:
    {
      struct CORE_MSG_MUSIC_START *m = &msg->music_start;
      mod_play_start(m->music, m->sample_freq, m->loop);
      audio_mixer_set_callback(mod_play_callback, m->volume);
    }
    break;

  case CORE_MSG_TYPE_MUSIC_STOP:
    {
      audio_mixer_set_callback(NULL, 0);
    }
    break;

  }

  printf("ERROR processing message: invalid messaged type %d\n", msg->msg_header.type);
}

void core1_main(void)
{
  multicore_fifo_push_blocking(0);
  multicore_fifo_pop_blocking();  // wait until core 0 is ready

  while (true) {
    sleep_ms(20);

    union CORE_MSG msg;
    while (core_msg_try_recv(&msg)) {
      process_message(&msg);
    }

    audio_mixer_step();
  }
}
