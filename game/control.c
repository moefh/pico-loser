
#include <stdio.h>

#include "control.h"
#include "game_data.h"
#include "character.h"
#include "collision.h"
#include "core_msg.h"

#define CAMERA_TETHER_X  40
#define CAMERA_TETHER_Y  60

#define SHOT_SPEED  10

static void move_shots(void)
{
  for (int i = 0; i < GAME_NUM_LOCAL_SHOTS; i++) {
    struct GAME_SHOT *shot = &game_local_shots[i];
    if (! shot->def) continue;

    int shot_dx = (shot->dir == GAME_DIR_LEFT) ? -SHOT_SPEED : SHOT_SPEED;
    int mdx, mdy;
    if (calc_movement(shot->x, shot->y, shot->def->clip.width, shot->def->clip.height, shot_dx, 0, &mdx, &mdy) != 0) {
      // collision: destroy shot
      shot->def = NULL;
      shot->spr->img = NULL;
      msg_audio_play_once(&game_sfx[GAME_NUM_SFX_EXPLOSION], 1<<8);
      continue;
    }
    shot->x += mdx;
    shot->y += mdy;
    shot->spr->x = shot->x - shot->def->clip.x;
    shot->spr->y = shot->y - shot->def->clip.y;
    shot->spr->frame = (shot->dir == GAME_DIR_LEFT) ? shot->def->mirror : 0;
  }
}

static void camera_follow_character(const struct GAME_CHAR *ch)
{
  int x, y;

  char_get_center(ch, &x, &y);
  if (game_data.camera_x < x - CAMERA_TETHER_X) game_data.camera_x = x - CAMERA_TETHER_X;
  if (game_data.camera_x > x + CAMERA_TETHER_X) game_data.camera_x = x + CAMERA_TETHER_X;
  if (game_data.camera_y < y - CAMERA_TETHER_Y) game_data.camera_y = y - CAMERA_TETHER_Y;
  if (game_data.camera_y > y + CAMERA_TETHER_Y) game_data.camera_y = y + CAMERA_TETHER_Y;
}

void control_update(struct JOYSTICK *joy)
{
  char_control(&game_local_player, joy);
  char_move(&game_local_player);
  move_shots();
  camera_follow_character(&game_local_player);
  
  joy->last = joy->cur;
}

void control_init(void)
{
  char_init(&game_local_player, &game_loserboy_def, &game_sprites[GAME_SPRITE_LOCAL_PLAYER], 64, 64, GAME_DIR_RIGHT);
}
