
#include "enemy_shadow.h"

static void update_sprite(struct GAME_ENEMY *enemy)
{
  int frame_base = 0;
  switch (enemy->state) {
  case GAME_CHAR_STATE_STAND:    frame_base = game_shadow_def.stand[(enemy->frame>>1) % game_shadow_def.num_stand]; break;
  case GAME_CHAR_STATE_WALK:     frame_base = game_shadow_def.walk [(enemy->frame>>1) % game_shadow_def.num_walk];  break;
  case GAME_CHAR_STATE_JUMP_START:
  case GAME_CHAR_STATE_JUMP_END: frame_base = game_shadow_def.jump [(enemy->frame>>1) % game_shadow_def.num_jump];  break;
  default:                       frame_base = 0; break;
  }
  enemy->spr->frame = frame_base + ((enemy->dir==GAME_DIR_LEFT) ? game_shadow_def.mirror : 0) + ((enemy->shooting_pose>0) ? game_shadow_def.shoot_frame : 0);
  enemy->spr->x = enemy->x >> 8;
  enemy->spr->y = enemy->y >> 8;
}

void enemy_shadow_init(struct GAME_ENEMY *enemy, struct GAME_SPRITE *spr, int x, int y, unsigned char dir)
{
  enemy->spr = spr;
  enemy->spawn_x = x;
  enemy->spawn_y = y;
  enemy->dir = dir;
  enemy->state = GAME_CHAR_STATE_STAND;
  enemy->frame = 0;
  enemy->shooting_pose = 0;
  enemy->tick = 0;

  enemy->x = enemy->spawn_x << 8;
  enemy->y = enemy->spawn_y << 8;
  update_sprite(enemy);
}

void enemy_shadow_move(struct GAME_ENEMY *enemy)
{
  if (enemy->tick > 0) {
    // thinking
    enemy->state = GAME_CHAR_STATE_STAND;
    enemy->frame = 0;
    if (--enemy->tick == 0) {
      enemy->dir = (enemy->dir == GAME_DIR_RIGHT) ? GAME_DIR_LEFT : GAME_DIR_RIGHT;
    }
  } else {
    // walking
    enemy->state = GAME_CHAR_STATE_WALK;
    enemy->frame++;
    if (enemy->dir == GAME_DIR_RIGHT) {
      enemy->x += 3<<8;
      if ((enemy->x >> 8) >= enemy->spawn_x + 100) {
        enemy->tick = 100;
      }
    } else {
      enemy->x -= 3<<8;
      if ((enemy->x >> 8) <= enemy->spawn_x - 100) {
        enemy->tick = 100;
      }
    }
  }
    
  update_sprite(enemy);
}
