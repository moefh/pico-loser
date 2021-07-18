
#include <string.h>

#include "audio_play.h"

struct AUDIO_SOURCE {
  int pos;
  int num_samples;
  const unsigned char *samples;
  int loop;
  int loop_start;
};

static struct AUDIO_SOURCE cur_sound;

void audio_play_once(const uint8_t *samples, int num_samples)
{
  cur_sound.samples = samples;
  cur_sound.num_samples = num_samples;
  cur_sound.pos = 0;
  cur_sound.loop = false;
}

void audio_play_loop(const uint8_t *samples, int num_samples, int loop_start)
{
  cur_sound.samples = samples;
  cur_sound.num_samples = num_samples;
  cur_sound.pos = 0;
  cur_sound.loop = true;
  cur_sound.loop_start = loop_start;
}

void audio_play_step(void)
{
  uint8_t *audio_buffer = audio_get_buffer();
  if (! audio_buffer) return;

  if (! cur_sound.samples) {
    memset(audio_buffer, 128, AUDIO_BUFFER_SIZE);
    return;
  }
  
  if (cur_sound.pos + AUDIO_BUFFER_SIZE < cur_sound.num_samples) {
    memcpy(audio_buffer, &cur_sound.samples[cur_sound.pos], AUDIO_BUFFER_SIZE);
    cur_sound.pos += AUDIO_BUFFER_SIZE;
  } else {
    int copy_len = cur_sound.num_samples - cur_sound.pos;
    memcpy(audio_buffer, &cur_sound.samples[cur_sound.pos], copy_len);
    memset(&audio_buffer[copy_len], 128, AUDIO_BUFFER_SIZE - copy_len);
    cur_sound.pos = cur_sound.num_samples;
  }
  if (cur_sound.pos == cur_sound.num_samples && cur_sound.loop) {
    cur_sound.pos = cur_sound.loop_start;
  }
}

