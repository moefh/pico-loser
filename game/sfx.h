#ifndef SFX_H_FILE
#define SFX_H_FILE

#include <stdint.h>

#include "game_data.h"

#ifdef __cplusplus
extern "C" {
#endif

void sfx_init(int sound_pin);
void sfx_play_once(const struct GAME_SFX *sfx, uint16_t volume);
void sfx_play_loop(const struct GAME_SFX *sfx, uint16_t volume);

#ifdef __cplusplus
}
#endif

#endif /* SFX_H_FILE */
