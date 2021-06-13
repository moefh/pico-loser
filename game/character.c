
#include "character.h"
#include "collision.h"

#define MAX_JUMP_SPEED   0x000e0000
#define START_JUMP_SPEED 0x000e0000
#define FALL_SPEED       0x00010000
#define INC_JUMP_SPEED   0x0000a000

#define MAX_WALK_SPEED   0x00070000
#define DEC_WALK_SPEED   0x0000b000

#define CTRL_FLAG_JUMP_START  (1u<<0)
#define CTRL_FLAG_JUMP_HOLD   (1u<<1)
#define CTRL_FLAG_SHOOT       (1u<<2)

static void create_new_shot(struct GAME_CHAR *ch)
{
  for (int i = 0; i < GAME_NUM_LOCAL_SHOTS; i++) {
    struct GAME_SHOT *shot = &game_local_shots[i];
    if (shot->def) continue;
    shot->def = &game_loserboy_shot_def;
    shot->spr = &game_sprites[i+GAME_NUM_SPRITE_FIRST_LOCAL_SHOT];
    shot->spr->img = &game_images[GAME_NUM_IMAGE_SHOT];
    shot->y = ch->y + 7;
    if (ch->dir == GAME_DIR_LEFT) {
      shot->x = ch->x - 2*shot->spr->img->width/3;
    } else {
      shot->x = ch->x + shot->spr->img->width/2;
    }
    shot->dir = ch->dir;
    break;
  }
}

static void control_stand(struct GAME_CHAR *ch, int joy_dx, unsigned int flags)
{
  if (flags & CTRL_FLAG_JUMP_START) {
    ch->state = GAME_CHAR_STATE_JUMP_START;
    ch->dy = -START_JUMP_SPEED;
    ch->frame = 0;
  } else if (joy_dx != 0) {
    ch->state = GAME_CHAR_STATE_WALK;
    ch->frame = 0;
  }
}

static void control_walk(struct GAME_CHAR *ch, int joy_dx, unsigned int flags)
{
  if (flags & CTRL_FLAG_JUMP_START) {
    ch->state = GAME_CHAR_STATE_JUMP_START;
    ch->dy = -START_JUMP_SPEED;
    ch->frame = 0;
  } else if (joy_dx == 0) {
    ch->state = GAME_CHAR_STATE_STAND;
    ch->frame = 0;
  }
}

static void control_jump_start(struct GAME_CHAR *ch, int joy_dx, unsigned int flags)
{
  if (flags & CTRL_FLAG_JUMP_HOLD) {
    ch->dy -= INC_JUMP_SPEED;
  } else {
    ch->state = GAME_CHAR_STATE_JUMP_END;
    ch->frame = 0;
  }
}

static void control_jump_end(struct GAME_CHAR *ch, int joy_dx, unsigned int flags)
{
  // nothing to do
}

void char_control(struct GAME_CHAR *ch, struct JOYSTICK *joy)
{
  int joy_dx = (joy->cur & JOY_BTN_LEFT) ? -1 : (joy->cur & JOY_BTN_RIGHT) ? 1 : 0;
  unsigned int control_flags = (((joy->cur  & JOY_BTN_A) ? CTRL_FLAG_JUMP_HOLD : 0) |
                                (((joy->cur & JOY_BTN_A) && ! (joy->last & JOY_BTN_A)) ? CTRL_FLAG_JUMP_START : 0) |
                                (((joy->cur & JOY_BTN_B) && ! (joy->last & JOY_BTN_B)) ? CTRL_FLAG_SHOOT : 0));
                                
  if (joy_dx > 0) {
    ch->dx += 2*DEC_WALK_SPEED;
    ch->dir = GAME_DIR_RIGHT;
  } else if (joy_dx < 0) {
    ch->dx -= 2*DEC_WALK_SPEED;
    ch->dir = GAME_DIR_LEFT;
  }
  if (ch->dx < -MAX_WALK_SPEED) ch->dx = -MAX_WALK_SPEED;
  if (ch->dx >  MAX_WALK_SPEED) ch->dx =  MAX_WALK_SPEED;

  if (control_flags & CTRL_FLAG_SHOOT) {
    create_new_shot(ch);
    ch->shooting_pose = 12;
  }
  
  switch (ch->state) {
  case GAME_CHAR_STATE_STAND:      control_stand     (ch, joy_dx, control_flags); break;
  case GAME_CHAR_STATE_WALK:       control_walk      (ch, joy_dx, control_flags); break;
  case GAME_CHAR_STATE_JUMP_START: control_jump_start(ch, joy_dx, control_flags); break;
  case GAME_CHAR_STATE_JUMP_END:   control_jump_end  (ch, joy_dx, control_flags); break;
  }
}

static void update_sprite(struct GAME_CHAR *ch)
{
  int frame_base = 0;
  switch (ch->state) {
  case GAME_CHAR_STATE_STAND:    frame_base = ch->def->stand[(ch->frame>>1) % ch->def->num_stand]; break;
  case GAME_CHAR_STATE_WALK:     frame_base = ch->def->walk [(ch->frame>>1) % ch->def->num_walk];  break;
  case GAME_CHAR_STATE_JUMP_START:
  case GAME_CHAR_STATE_JUMP_END: frame_base = ch->def->jump [(ch->frame>>1) % ch->def->num_jump];  break;
  default:                       frame_base = 0; break;
  }
  ch->spr->frame = frame_base + ((ch->dir==GAME_DIR_LEFT) ? ch->def->mirror : 0) + ((ch->shooting_pose>0) ? ch->def->shoot_frame : 0);
  ch->spr->x = ch->x + ((ch->dir == GAME_DIR_RIGHT) ? -ch->def->clip.x : ch->def->clip.x + ch->def->clip.width - ch->spr->img->width - 1);
  ch->spr->y = ch->y - ch->def->clip.y;
}

static void decrease_horizontal_speed(struct GAME_CHAR *ch, int amount)
{
  int sign;

  if (ch->dx >= 0) {
    sign = 1;
  } else {
    ch->dx = -ch->dx;
    sign = -1;
  }
  ch->dx -= amount;
  if (ch->dx <= 0) {
    ch->dx = 0;
    if (ch->state == GAME_CHAR_STATE_WALK) {
      ch->state = GAME_CHAR_STATE_STAND;
      ch->frame = 0;
    }
  } else if (sign < 0) {
    ch->dx = -ch->dx;
  }
}

void char_move(struct GAME_CHAR *ch)
{
  int mdx, mdy;
  if (ch->state == GAME_CHAR_STATE_STAND || ch->state == GAME_CHAR_STATE_WALK) {
    // check floor under character
    calc_movement(ch->x, ch->y, ch->def->clip.width, ch->def->clip.height, 0, 1, &mdx, &mdy);
    if (mdy > 0) {
      // start falling
      ch->state = GAME_CHAR_STATE_JUMP_END;
      ch->frame = 0;
    }
  }

  if (ch->state != GAME_CHAR_STATE_STAND && ch->state != GAME_CHAR_STATE_WALK) {
    ch->dy += FALL_SPEED;
  }

  int flags = calc_movement(ch->x, ch->y, ch->def->clip.width, ch->def->clip.height, ch->dx/0x10000, ch->dy/0x10000, &mdx, &mdy);
  if (flags & CM_Y_CLIPPED) {
    if (ch->dy > 0) {      /* Hit the ground */
      //state = (joy->cur & (JOY_BTN_LEFT|JOY_BTN_RIGHT)) ? GAME_CHAR_STATE_WALK : GAME_CHAR_STATE_STAND;
      ch->state = GAME_CHAR_STATE_WALK;
      ch->dy = 0;
      ch->frame = 0;
    } else {           /* Hit the ceiling */
      ch->dy = 0;
      ch->state = GAME_CHAR_STATE_JUMP_END;
    }
  }
  if (flags & CM_X_CLIPPED) {
    decrease_horizontal_speed(ch, DEC_WALK_SPEED / 2);
  }

  ch->x += mdx;
  ch->y += mdy;
  if (ch->x < 0) ch->x = 0;
  if (ch->y < 0) ch->y = 0;

  switch (ch->state) {
  case GAME_CHAR_STATE_STAND:  /* ??? */
  case GAME_CHAR_STATE_WALK:
    decrease_horizontal_speed(ch, DEC_WALK_SPEED);
    break;

  case GAME_CHAR_STATE_JUMP_START:
    if (ch->dy < 0 && ch->dy > -FALL_SPEED)
      ch->state = GAME_CHAR_STATE_JUMP_END;
    if (ch->dy > MAX_JUMP_SPEED)
      ch->dy = MAX_JUMP_SPEED;
    break;

  case GAME_CHAR_STATE_JUMP_END:
    if (ch->dy > MAX_JUMP_SPEED)
      ch->dy = MAX_JUMP_SPEED;
    break;
  }

  ch->frame++;
  if (ch->shooting_pose > 0) {
    ch->shooting_pose--;
  }

  update_sprite(ch);
}

void char_get_center(const struct GAME_CHAR *ch, int *x, int *y)
{
  *x = ch->x + ch->def->clip.width/2;
  *y = ch->y + ch->def->clip.height/2;
}

void char_init(struct GAME_CHAR *ch, const struct GAME_CHAR_DEF *def, struct GAME_SPRITE *spr, int x, int y, unsigned char dir)
{
  ch->def = def;
  ch->spr = spr;
  ch->x = x;
  ch->y = y;
  ch->dir = dir;
  ch->dx = 0;
  ch->dy = 0;
  ch->state = GAME_CHAR_STATE_JUMP_END;
  ch->shooting_pose = 0;
}
