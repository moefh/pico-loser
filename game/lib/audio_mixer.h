#ifndef AUDIO_MIXER_H_FILE
#define AUDIO_MIXER_H_FILE

#include <stdint.h>

#define AUDIO_MIXER_MAX_SOURCES 4

typedef void (*audio_callback)(uint8_t *samples, uint32_t len);

#ifdef __cplusplus
extern "C" {
#endif

void audio_mixer_init(void);
void audio_mixer_step(void);

void audio_mixer_set_callback(audio_callback func, uint16_t volume);

int audio_mixer_once(const uint8_t *samples, uint32_t len);
int audio_mixer_loop(const uint8_t *samples, uint32_t len, uint32_t loop_start);

void audio_mixer_stop_source(int source_id);
void audio_mixer_set_source_volume(int source_id, uint16_t volume);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MIXER_H_FILE */
