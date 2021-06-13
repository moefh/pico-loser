
#include <stdio.h>
#include "pico/stdlib.h"

#include "lib/wii_i2c.h"

#include "joy_wii_i2c.h"

struct JOY_HW {
  i2c_inst_t *i2c;
  unsigned int ident;
  int update_state;
};

static struct JOY_HW joy_hw[2];
static unsigned int num_hw_used;

static const char *get_name(unsigned int ident)
{
  switch (ident) {
  case WII_I2C_IDENT_NONE:    return "none";
  case WII_I2C_IDENT_NUNCHUK: return "Wii Nunchuk";
  case WII_I2C_IDENT_CLASSIC: return "Wii Classic";
  default:                    return "Wii Unknown";
  }
}

static void req_state(struct JOYSTICK *joy)
{
  struct JOY_HW *hw = joy->hw;
  wii_i2c_request_state(hw->i2c);
}

static void read_state(struct JOYSTICK *joy)
{
  struct JOY_HW *hw = joy->hw;
  unsigned char state[6];
  if (wii_i2c_read_state(hw->i2c, state) < 0) {
    return;
  }

  if (state[0] == 0 && state[1] == 0 && state[2] == 0 &&
      state[3] == 0 && state[4] == 0 && state[5] == 0) {
    return;
  }
  
  switch (hw->ident) {
  case WII_I2C_IDENT_NUNCHUK:
    {
      struct wii_i2c_nunchuk_state nun;
      wii_i2c_decode_nunchuk(state, &nun);
      joy->cur = (((nun.x < -20) ? JOY_BTN_LEFT  : 0) |
                  ((nun.x >  20) ? JOY_BTN_RIGHT : 0) |
                  ((nun.y >  20) ? JOY_BTN_UP    : 0) |
                  ((nun.y < -20) ? JOY_BTN_DOWN  : 0) |
                  (nun.c         ? JOY_BTN_A     : 0) |
                  (nun.z         ? JOY_BTN_B     : 0));
    }
    break;

  case WII_I2C_IDENT_CLASSIC:
    {
      struct wii_i2c_classic_state cl;
      wii_i2c_decode_classic(state, &cl);
      joy->cur = ((cl.left  ? JOY_BTN_LEFT  : 0) |
                  (cl.right ? JOY_BTN_RIGHT : 0) |
                  (cl.up    ? JOY_BTN_UP    : 0) |
                  (cl.down  ? JOY_BTN_DOWN  : 0) |
                  (cl.b     ? JOY_BTN_A     : 0) |
                  (cl.y     ? JOY_BTN_B     : 0) |
                  (cl.a     ? JOY_BTN_C     : 0) |
                  (cl.x     ? JOY_BTN_D     : 0) |
                  (cl.minus ? JOY_BTN_E     : 0) |
                  (cl.plus  ? JOY_BTN_F     : 0));
    }
    break;

  default:
    joy->cur = 0;
  }
}

static bool auto_update_timer_callback(repeating_timer_t *t)
{
  joy_wii_i2c_update(t->user_data);
  return true;
}

// === INTERFACE ====================================================

int joy_wii_i2c_init(struct JOYSTICK *joy, unsigned int sda_pin, unsigned int scl_pin)
{
  joy->enabled = 0;
  joy->name = get_name(WII_I2C_IDENT_NONE);

  struct JOY_HW *hw = joy->hw;
  if (! hw) {
    if (num_hw_used >= count_of(joy_hw)) return -1;
    hw = &joy_hw[num_hw_used];
    hw->i2c = (num_hw_used == 0) ? i2c0 : i2c1;
    hw->ident = WII_I2C_IDENT_NONE;
    hw->update_state = (num_hw_used == 0) ? -1 : -2; // 2nd joy will update 1 cycle after 1st
    num_hw_used++;
    joy->hw = hw;
  }
  
  if (wii_i2c_init(hw->i2c, sda_pin, scl_pin) < 0) return -1;
  sleep_ms(1);
  if (wii_i2c_read_ident(hw->i2c, &hw->ident) < 0) return -1;

  joy->enabled = 1;
  joy->name = get_name(hw->ident);

  return 0;
}

void joy_wii_i2c_start_auto_update(struct JOYSTICK *joy)
{
  repeating_timer_t timer;
  add_repeating_timer_ms(20, auto_update_timer_callback, joy, &timer);
}

void joy_wii_i2c_request_state(struct JOYSTICK *joy)
{
  req_state(joy);
}

void joy_wii_i2c_read_state(struct JOYSTICK *joy)
{
  read_state(joy);
}

void joy_wii_i2c_update(struct JOYSTICK *joy)
{
  struct JOY_HW *hw = joy->hw;
  if (hw->ident == WII_I2C_IDENT_NONE) {
    return;
  }

  switch (hw->update_state) {
  case  0: req_state(joy);   hw->update_state++;   break;
  case  2: read_state(joy);  hw->update_state++;   break;
  default:
    if (hw->update_state < 2)
      hw->update_state++;
    else
      hw->update_state = 0;
    break;
  }
}
