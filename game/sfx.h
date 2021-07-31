#ifndef SFX_H_FILE
#define SFX_H_FILE

#include <stdint.h>
#include <stdbool.h>

#include "game_data.h"

#ifdef __cplusplus
extern "C" {
#endif

void sfx_init(int sound_pin, int sample_freq);
void sfx_play_once(const struct GAME_SFX *sfx, uint16_t volume);
void sfx_play_loop(const struct GAME_SFX *sfx, uint16_t volume);
void sfx_music_start(const struct MOD_DATA *music, uint16_t volume, bool loop);
void sfx_music_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SFX_H_FILE */
