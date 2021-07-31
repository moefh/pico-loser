#include <string.h>
#include <stdio.h>

#include "audio_mixer.h"
#include "audio.h"

struct DATA_SOURCE {
  const uint8_t *pos;
  const uint8_t *loop_start;
  const uint8_t *end;
  uint16_t volume; // 8.8 fixed point (1.0 = 0x0100 = 100%)
};

static struct DATA_SOURCE data_sources[AUDIO_MIXER_MAX_SOURCES];

static audio_callback callback_source;
static uint16_t callback_volume;
static uint8_t callback_buffer[AUDIO_BUFFER_SIZE];

static int audio_find_unused_source(void)
{
  for (int i = 0; i < AUDIO_MIXER_MAX_SOURCES; i++) {
    if (! data_sources[i].pos) {
      return i;
    }
  }
  return -1;
}

void audio_mixer_init(void)
{
  audio_mixer_set_callback(NULL, 0);
}

void audio_mixer_set_callback(audio_callback func, uint16_t volume)
{
  callback_source = func;
  callback_volume = volume;
  if (! callback_source) {
    memset(callback_buffer, 128, AUDIO_BUFFER_SIZE);
  }
}

int audio_mixer_once(const uint8_t *samples, uint32_t len)
{
  int source_id = audio_find_unused_source();
  if (source_id < 0) {
    return -1;
  }
  struct DATA_SOURCE *source = &data_sources[source_id];
  source->pos = samples;
  source->loop_start = samples + len;
  source->end = samples + len;
  source->volume = 256;
  return source_id;
}

int audio_mixer_loop(const uint8_t *samples, uint32_t len, uint32_t loop_start)
{
  int source_id = audio_find_unused_source();
  if (source_id < 0) {
    return -1;
  }
  struct DATA_SOURCE *source = &data_sources[source_id];
  source->pos = samples;
  source->loop_start = samples + loop_start;
  source->end = samples + len;
  source->volume = 256;
  return source_id;
}

void audio_mixer_stop_source(int source_id)
{
  data_sources[source_id].pos = NULL;
}

void audio_mixer_set_source_volume(int source_id, uint16_t volume)
{
  data_sources[source_id].volume = volume;
}

static uint8_t clamp(int sample)
{
  if (sample > 255) return 255;
  if (sample <   0) return 0;
  return sample;
}

void audio_mixer_step(void)
{
  uint8_t *audio_buffer = audio_get_buffer();
  if (! audio_buffer) return;

  // run callback source if we have one
  uint16_t callback_volume_local = 0;
  if (callback_source) {
    callback_source(callback_buffer, AUDIO_BUFFER_SIZE);
    callback_volume_local = callback_volume;
  }
  
  for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
    int sample = 0;

    // mix sample sources
    for (int s = 0; s < AUDIO_MIXER_MAX_SOURCES; s++) {
      struct DATA_SOURCE *source = &data_sources[s];
      if (! source->pos) continue;

      sample += (*source->pos - 128) * source->volume;
      if (++source->pos >= source->end) {
        if (source->loop_start < source->end) {
          source->pos = source->loop_start;
        } else {
          source->pos = NULL;
        }
      }
    }

    // mix callback source
    sample += (callback_buffer[i] - 128) * callback_volume_local;
    
    // clamp mix result
    audio_buffer[i] = clamp((sample>>8) + 128);
  }
}
