#ifndef AUDIO_PLAY_H_FILE
#define AUDIO_PLAY_H_FILE

#include <stdint.h>
#include <stdbool.h>

#include "audio.h"

void audio_play_once(const uint8_t *samples, int num_samples);
void audio_play_loop(const uint8_t *samples, int num_samples, int loop_start);

void audio_play_step(void);

#endif /* AUDIO_PLAY_H_FILE */
