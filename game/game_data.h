#ifndef GAME_DATA_H_FILE
#define GAME_DATA_H_FILE

#include "lib/vga_font.h"
#include "lib/vga_image.h"
#include "lib/mod_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TILE_WIDTH  64
#define TILE_HEIGHT 64

#define GAME_DIR_LEFT  0
#define GAME_DIR_RIGHT 1

#define GAME_NUM_IMAGE_TILES               0   // image index for map tiles
#define GAME_NUM_IMAGE_LOSERBOY            1   // image index for loserboy
#define GAME_NUM_IMAGE_SHOT                2   // image index for shot

#define GAME_NUM_LOCAL_SHOTS               7
#define GAME_NUM_REMOTE_SHOTS              7
#define GAME_NUM_SHOTS                     (GAME_NUM_LOCAL_SHOTS+GAME_NUM_REMOTE_SHOTS)

#define GAME_SPRITE_LOCAL_PLAYER           0   // sprite index for local player character
#define GAME_SPRITE_REMOTE_PLAYER          1   // sprite index for remote player character
#define GAME_NUM_SPRITE_FIRST_LOCAL_SHOT   2   // sprite index for first local shot
#define GAME_NUM_SPRITE_FIRST_REMOTE_SHOT  (GAME_NUM_SPRITE_FIRST_LOCAL_SHOT+GAME_NUM_LOCAL_SHOTS)
#define GAME_NUM_SPRITES                   (2+GAME_NUM_SHOTS)

#define GAME_NUM_SFX_BUMP                  0
#define GAME_NUM_SFX_EXPLOSION             1

enum {
  MAP_BLOCK,
  MAP_BLOCK1,
  MAP_BLOCK2,
  MAP_BLOCK3,
  MAP_BLOCK4,
  MAP_BLOCK5,
  MAP_BLOCK6,
  MAP_BLOCK7,
  MAP_BLOCK8,
  MAP_BLOCK9,
  MAP_BLOCK10,
  MAP_BLOCK11,
  MAP_BLOCK12,
  MAP_BLOCK13,
  MAP_BLOCK14,

  MAP_SECRET,

  MAP_STARTRIGHT,
  MAP_STARTLEFT,

  N_MAP_BLOCKS,
};

struct GAME_MAP_TILE {
  unsigned short back;
  unsigned short fore;
  unsigned short block;
};

struct GAME_MAP_POINT {
  unsigned int x;     // fixed 16.16
  unsigned int y;     // fixed 16.16
};

struct GAME_MAP_SPAWN_POINT {
  struct GAME_MAP_POINT pos;
  unsigned char dir;  // 0=left, 1=right
};

struct GAME_MAP {
  const struct VGA_IMAGE *tileset;
  const struct GAME_MAP_TILE *tiles;
  const struct GAME_MAP_SPAWN_POINT *spawn_points;
  unsigned char num_spawn_points;
  unsigned short width;
  unsigned short height;
};

struct GAME_SPRITE {
  const struct VGA_IMAGE *img;
  short x;
  short y;
  short frame;
};

struct GAME_DATA {
  short int camera_x;
  short int camera_y;
};

struct GAME_RECT {
  unsigned char x;
  unsigned char y;
  unsigned char width;
  unsigned char height;
};

struct GAME_CHAR_DEF {
  struct GAME_RECT clip;
  unsigned char mirror;
  unsigned char shoot_frame;
  unsigned char num_stand;
  unsigned char stand[4];
  unsigned char num_jump;
  unsigned char jump[4];
  unsigned char num_walk;
  unsigned char walk[64];
};

struct GAME_CHAR {
  const struct GAME_CHAR_DEF *def;
  struct GAME_SPRITE *spr;
  int x;
  int y;
  int dx;
  int dy;
  unsigned char dir;
  unsigned char state;
  unsigned char frame;
  unsigned char shooting_pose;
};

struct GAME_SHOT_DEF {
  const struct GAME_RECT clip;
  unsigned char mirror;
};

struct GAME_SHOT {
  const struct GAME_SHOT_DEF *def;
  struct GAME_SPRITE *spr;
  unsigned short x;
  unsigned short y;
  unsigned char dir;
};

struct GAME_SFX {
  const unsigned char *samples;
  unsigned int len;
  unsigned int loop_start;
};

extern const struct VGA_IMAGE game_images[];
extern const struct VGA_FONT font6x8;
extern const struct GAME_MAP game_map;
extern const struct GAME_CHAR_DEF game_loserboy_def;
extern const struct GAME_SHOT_DEF game_loserboy_shot_def;
extern const struct GAME_SFX game_sfx[];
extern const struct MOD_DATA *game_music;

extern struct GAME_SPRITE game_sprites[];
extern struct GAME_DATA game_data;
extern struct GAME_CHAR game_local_player;
extern struct GAME_CHAR game_remote_player;
extern struct GAME_SHOT game_local_shots[];
extern struct GAME_SHOT game_remote_shots[];

#ifdef __cplusplus
}
#endif

#endif /* GAME_DATA_H_FILE */
