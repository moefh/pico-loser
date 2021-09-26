#ifndef ENEMY_SHADOW_H_FILE
#define ENEMY_SHADOW_H_FILE

#include "game_data.h"

#ifdef __cplusplus
extern "C" {
#endif

void enemy_shadow_init(struct GAME_ENEMY *enemy, struct GAME_SPRITE *spr, int x, int y, unsigned char dir);
void enemy_shadow_move(struct GAME_ENEMY *enemy);

#ifdef __cplusplus
}
#endif

#endif /* ENEMY_SHADOW_H_FILE */
