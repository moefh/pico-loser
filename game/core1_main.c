#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "core_msg.h"
#include "lib/audio.h"

static void process_message(union CORE_MSG *msg)
{
  //printf("processing message of type %d\n", msg->msg_header.type);
  switch (msg->msg_header.type) {
  case CORE_MSG_TYPE_AUDIO_INIT:
    {
      struct CORE_MSG_AUDIO_INIT *m = &msg->audio_init;
      audio_init(m->audio_pin);
    }
    return;

  case CORE_MSG_TYPE_AUDIO_PLAY_ONCE:
    {
      struct CORE_MSG_AUDIO_PLAY_ONCE *m = &msg->audio_play_once;
      int source_id = audio_play_once(m->samples, m->len);
      if (source_id >= 0) {
        audio_source_set_volume(source_id, m->volume);
      }
    }
    return;

  case CORE_MSG_TYPE_AUDIO_PLAY_LOOP:
    {
      struct CORE_MSG_AUDIO_PLAY_LOOP *m = &msg->audio_play_loop;
      int source_id = audio_play_loop(m->samples, m->len, m->loop_start);
      if (source_id >= 0) {
        audio_source_set_volume(source_id, m->volume);
      }
    }
    return;
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
