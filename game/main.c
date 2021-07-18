#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"

#include "core_msg.h"

#include "joystick.h"
#include "joy_wii_i2c.h"

#include "screen.h"
#include "control.h"
#include "sfx.h"

#define VGA_PIN_BASE  2
#define JOY_SDA       12
#define JOY_SCL       13
#define AUDIO_OUT     16

void core1_main(void);

static struct JOYSTICK joy;

static void blink_led(void)
{
  static int led_state = 1;
  static int frame_count = 0;
  if (frame_count++ >= 15) {
    frame_count = 0;
    led_state = !led_state;
    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
  }
}

int main(void)
{
  // pin names
  bi_decl(bi_pin_mask_with_name(3 << (VGA_PIN_BASE + 0), "VGA Blue 0-1"));
  bi_decl(bi_pin_mask_with_name(3 << (VGA_PIN_BASE + 2), "VGA Green 0-1"));
  bi_decl(bi_pin_mask_with_name(3 << (VGA_PIN_BASE + 4), "VGA Red 0-1"));
  bi_decl(bi_1pin_with_name(VGA_PIN_BASE + 6, "VGA H-Sync"));
  bi_decl(bi_1pin_with_name(VGA_PIN_BASE + 7, "VGA V-Sync"));
  bi_decl(bi_1pin_with_name(JOY_SDA, "Joystick SDA"));
  bi_decl(bi_1pin_with_name(JOY_SCL, "Joystick SCL"));
  bi_decl(bi_1pin_with_name(AUDIO_OUT, "Audio output"));
  
  stdio_init_all();
  //sleep_ms(5000); printf("=====\nstarting\n");

  // start core 1
  core_msg_init();
  multicore_launch_core1(core1_main);
  multicore_fifo_pop_blocking();  // wait for core1 to start
  multicore_fifo_push_blocking(0);

  // start LED output
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  //sleep_ms(5000); printf("Starting...\n");

  joy_wii_i2c_init(&joy, JOY_SDA, JOY_SCL);
  sfx_init(AUDIO_OUT);

  if (screen_init(VGA_PIN_BASE) < 0) {
    return 1;
  }

  control_init(&joy);
  while (true) {
    blink_led();
    joy_wii_i2c_update(&joy);
    control_update(&joy);
    screen_render(&joy);
  }
}
