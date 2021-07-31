#ifndef AUDIO_H_FILE
#define AUDIO_H_FILE

#define AUDIO_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

void audio_init(int audio_pin, int sample_freq);
uint8_t *audio_get_buffer(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_H_FILE */
