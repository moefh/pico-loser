
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "lib/vga_6bit.h"
#include "lib/vga_font.h"
#include "lib/vga_image.h"

#include "screen.h"
#include "game_data.h"
#include "compilation_datetime.h"

#pragma GCC optimize ("-O3")

#define TILE_WIDTH  64
#define TILE_HEIGHT 64
#define NUM_TILES_CACHE 5

static int screen_x = 0;
static int screen_y = 0;

#if NUM_TILES_CACHE > 0
// place the first few tiles in RAM for faster drawing
static unsigned int tile_cache[NUM_TILES_CACHE*TILE_WIDTH*TILE_HEIGHT/sizeof(unsigned int)];
static struct VGA_IMAGE tile_cache_image = { 64, 64, 64/sizeof(unsigned int), tile_cache };
#endif

int screen_init(int vga_pin_base)
{
  if (vga_init(&vga_mode_320x240, vga_pin_base) < 0) {
    printf("ERROR initializing VGA\n");
    fflush(stdout);
    return -1;
  }

  font_set_font(&font6x8);
  font_set_color(0x3f);
  game_data.camera_x = 0;
  game_data.camera_y = 0;

#if NUM_TILES_CACHE > 0
  memcpy(tile_cache, game_map.tileset->data, NUM_TILES_CACHE*TILE_WIDTH*TILE_HEIGHT);
#endif

  return 0;
}

static void set_screen_pos(void)
{
  screen_x = game_data.camera_x - vga_screen.width/2;
  screen_y = game_data.camera_y - vga_screen.height/2;

  if (screen_x < 0) {
    screen_x = 0;
  } else if (screen_x >= game_map.width*TILE_WIDTH - vga_screen.width) {
    screen_x = game_map.width*TILE_WIDTH - vga_screen.width - 1;
  }

  if (screen_y < 0) {
    screen_y = 0;
  } else if (screen_y >= game_map.height*TILE_HEIGHT - vga_screen.height) {
    screen_y = game_map.height*TILE_HEIGHT - vga_screen.height - 1;
  }
}

static void render_game_frame(void)
{
  set_screen_pos();

  int tile_x_first = screen_x/TILE_WIDTH;
  int tile_x_last = (screen_x+vga_screen.width)/TILE_WIDTH;
  int tile_y_first = screen_y/TILE_HEIGHT;
  int tile_y_last = (screen_y+vga_screen.height)/TILE_HEIGHT;

  int x_pos_start = -(screen_x%TILE_WIDTH);
  int y_pos_start = -(screen_y%TILE_HEIGHT);

  int y_pos;

  // background
  y_pos = y_pos_start;
  for (int tile_y = tile_y_first; tile_y <= tile_y_last; tile_y++) {
    int x_pos = x_pos_start;
    const struct GAME_MAP_TILE *tiles = &game_map.tiles[tile_y*game_map.width];
    for (int tile_x = tile_x_first; tile_x <= tile_x_last; tile_x++) {
      int tile_num = tiles[tile_x].back;
      if (tile_num != 0xffff) {
#if NUM_TILES_CACHE > 0
        if (tile_num < NUM_TILES_CACHE)
          vga_image_draw_frame(&tile_cache_image, tile_num, x_pos, y_pos, false);
        else
#endif
          vga_image_draw_frame(game_map.tileset, tile_num, x_pos, y_pos, false);
      }
      x_pos += TILE_WIDTH;
    }
    y_pos += TILE_HEIGHT;
  }

  // sprites
  for (int i = 0; i < GAME_NUM_SPRITES; i++) {
    if (! game_sprites[i].img) continue;
    int spr_x = game_sprites[i].x - screen_x;
    int spr_y = game_sprites[i].y - screen_y;
    if (spr_x <= -game_sprites[i].img->width) continue;
    if (spr_y <= -game_sprites[i].img->height) continue;
    if (spr_x >= vga_screen.width || spr_y >= vga_screen.height) continue;
    vga_image_draw_frame(game_sprites[i].img, game_sprites[i].frame, spr_x, spr_y, true);
  }

  // foreground
  y_pos = y_pos_start;
  for (int tile_y = tile_y_first; tile_y <= tile_y_last; tile_y++) {
    int x_pos = x_pos_start;
    const struct GAME_MAP_TILE *tiles = &game_map.tiles[tile_y*game_map.width];
    for (int tile_x = tile_x_first; tile_x <= tile_x_last; tile_x++) {
      int tile_num = tiles[tile_x].fore;
      if (tile_num != 0xffff) {
#if NUM_TILES_CACHE > 0
        if (tile_num < NUM_TILES_CACHE)
          vga_image_draw_frame(&tile_cache_image, tile_num, x_pos, y_pos, true);
        else
#endif
          vga_image_draw_frame(game_map.tileset, tile_num, x_pos, y_pos, true);
      }
      x_pos += TILE_WIDTH;
    }
    y_pos += TILE_HEIGHT;
  }
}

static int count_fps(int cur_millis, int last_millis)
{
  static int last_fps;
  static int fps_frame_count;
  
  if (cur_millis/1000 != last_millis/1000) {
    last_fps = fps_frame_count;
    fps_frame_count = 1;
  } else {
    fps_frame_count++;
  }
  return last_fps;
}

void screen_render(struct JOYSTICK *joy)
{
  static int last_millis;

  //vga_clear_screen(0x03);
  render_game_frame();

  int cur_millis = to_ms_since_boot(get_absolute_time());

  font_align(FONT_ALIGN_LEFT);
  font_move(10, 10); font_printf("%d fps", count_fps(cur_millis, last_millis));
  
  font_align(FONT_ALIGN_RIGHT);
  font_move(vga_screen.width-10, 10); font_printf("x = %-4d", game_local_player.x);
  font_move(vga_screen.width-10, 20); font_printf("y = %-4d", game_local_player.y);

  font_align(FONT_ALIGN_LEFT);
  font_move(10, vga_screen.height-30); font_printf("Joy: %s", joy->name);
  font_move(10, vga_screen.height-20);
  if (joy->cur & JOY_BTN_UP)    font_print("up ");
  if (joy->cur & JOY_BTN_DOWN)  font_print("down ");
  if (joy->cur & JOY_BTN_LEFT)  font_print("left ");
  if (joy->cur & JOY_BTN_RIGHT) font_print("right ");
  if (joy->cur & JOY_BTN_A)     font_print("a ");
  if (joy->cur & JOY_BTN_B)     font_print("b ");
  if (joy->cur & JOY_BTN_C)     font_print("c ");
  if (joy->cur & JOY_BTN_D)     font_print("d ");
  if (joy->cur & JOY_BTN_E)     font_print("e ");
  if (joy->cur & JOY_BTN_F)     font_print("f ");

  font_align(FONT_ALIGN_RIGHT);
  font_move(vga_screen.width-10, vga_screen.height-30); font_printf("build");
  font_move(vga_screen.width-10, vga_screen.height-20); font_printf("%08x", get_compilation_date());
  
  vga_swap_buffers(true);
  last_millis = cur_millis;
}
