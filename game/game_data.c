
#include "game_data.h"

// fonts
#include "data/font6x8.h"

// maps
#include "data/map.h"

// sprites
#include "data/spr_castle3.h"
#include "data/spr_loserboy.h"
#include "data/spr_pwr2.h"

// sounds
#include "data/snd_bump.h"
#include "data/snd_explosion.h"

/**
 * ==================================================================
 * ==== READ-ONLY DATA (FLASH) ======================================
 * ==================================================================
 */

const struct VGA_FONT font6x8 = {
  .w = font6x8_width,
  .h = font6x8_height,
  .first_char = font6x8_first_char,
  .num_chars = font6x8_num_chars,
  .data = font6x8_data,
};

const struct GAME_MAP game_map = {
  .width = game_map_width,
  .height = game_map_height,
  .num_spawn_points = sizeof(game_map_spawn_points)/sizeof(game_map_spawn_points[0]),
  .spawn_points = game_map_spawn_points,
  .tiles = game_map_tiles,
  .tileset = &game_images[GAME_NUM_IMAGE_TILES],
};

const struct VGA_IMAGE game_images[] = {
#define ADD_IMAGE(name) { img_##name##_width, img_##name##_height, img_##name##_stride, img_##name##_data }
  ADD_IMAGE(castle3),
  ADD_IMAGE(loserboy),
  ADD_IMAGE(pwr2),
#undef ADD_IMAGE
};

const struct GAME_CHAR_DEF game_loserboy_def = {
  .clip = { 15, 5, 31, 35 },
  .mirror = 11,
  .shoot_frame = 22,
  .num_stand = 1,
  .stand = { 10 },
  .num_jump = 1,
  .jump = { 4 },
  .num_walk = 36,
  .walk = {
    5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5,
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0,
  },
};

const struct GAME_SHOT_DEF game_loserboy_shot_def = {
  .clip = { 10, 10, 34, 23 },
  .mirror = 1,
};

const struct GAME_SFX game_sfx[] = {
#define ADD_SFX(name) { .samples = snd_##name, .len = sizeof(snd_##name), 0 }
  ADD_SFX(bump),
  ADD_SFX(explosion),
#undef ADD_SFX
};

/**
 * ==================================================================
 * ==== MUTABLE DATA ================================================
 * ==================================================================
 */

struct GAME_DATA game_data;
struct GAME_CHAR game_local_player;
struct GAME_CHAR game_remote_player;

struct GAME_SPRITE game_sprites[GAME_NUM_SPRITES] = {
  { &game_images[GAME_NUM_IMAGE_LOSERBOY],  64,  64,  0, },   // local player
  { &game_images[GAME_NUM_IMAGE_LOSERBOY], -64, -64,  0, },   // remote plater (over network)
};

struct GAME_SHOT game_local_shots[GAME_NUM_LOCAL_SHOTS];
struct GAME_SHOT game_remote_shots[GAME_NUM_REMOTE_SHOTS];
