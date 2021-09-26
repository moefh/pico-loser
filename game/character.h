#ifndef CHARACTER_H_FILE
#define CHARACTER_H_FILE

#include "game_data.h"
#include "joystick.h"

#ifdef __cplusplus
extern "C" {
#endif

void char_init(struct GAME_CHAR *ch, const struct GAME_CHAR_DEF *def, struct GAME_SPRITE *spr, int x, int y, unsigned char dir);
void char_control(struct GAME_CHAR *ch, struct JOYSTICK *joy);
void char_move(struct GAME_CHAR *ch);
void char_get_center(const struct GAME_CHAR *ch, int *x, int *y);

#ifdef __cplusplus
}
#endif

#endif /* CHARACTER_H_FILE */
