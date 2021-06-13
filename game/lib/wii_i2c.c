#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "wii_i2c.h"

#define WII_I2C_ADDR  0x52

static const uint8_t data_init1[]     = { 0xf0, 0x55 };
static const uint8_t data_init2[]     = { 0xfb, 0x00 };
static const uint8_t data_req_ident[] = { 0xfa };
static const uint8_t data_req_data[]  = { 0x00 };

#define CHECK(ret, expect, msg) do {                   \
    if (ret != expect) {                               \
      dump_error(msg, ret);                            \
      return ret;                                      \
    }                                                  \
  } while (0)

static void dump_error(const char *msg, int err)
{
  printf("ERROR in %s: ", msg);
  switch (err) {
  case PICO_ERROR_GENERIC: printf("no device present\n"); break;
  case PICO_ERROR_TIMEOUT: printf("timeout\n"); break;
  default: printf("unknown error: %d\n", err); break;
  }
}

int wii_i2c_init(i2c_inst_t *i2c, unsigned int sda_pin, unsigned int scl_pin)
{
  i2c_init(i2c, 100 * 1000);
  gpio_set_function(sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(scl_pin, GPIO_FUNC_I2C);
  gpio_pull_up(sda_pin);
  gpio_pull_up(scl_pin);

  int ret;
  ret = i2c_write_timeout_us(i2c, WII_I2C_ADDR, data_init1, sizeof(data_init1), false, 10000); CHECK(ret, sizeof(data_init1), "init1");
  sleep_ms(1);
  ret = i2c_write_timeout_us(i2c, WII_I2C_ADDR, data_init2, sizeof(data_init2), false, 2000); CHECK(ret, sizeof(data_init2), "init2");

  return 0;
}

int wii_i2c_read_ident(i2c_inst_t *i2c, unsigned int *ident)
{
  *ident = WII_I2C_IDENT_NONE;
  
  int ret = i2c_write_timeout_us(i2c, WII_I2C_ADDR, data_req_ident, sizeof(data_req_ident), false, 2000);
  CHECK(ret, sizeof(data_req_ident), "req_ident");

  sleep_ms(1);
  
  uint8_t data[6];
  ret = i2c_read_timeout_us(i2c, WII_I2C_ADDR, data, sizeof(data), false, 2000);
  CHECK(ret, sizeof(data), "read_ident");
  
  *ident = (((unsigned int)data[5] <<  0) |
            ((unsigned int)data[4] <<  8) |
            ((unsigned int)data[3] << 16) |
            ((unsigned int)data[2] << 24));
  return 0;
}

int wii_i2c_request_state(i2c_inst_t *i2c)
{
  int ret = i2c_write_timeout_us(i2c, WII_I2C_ADDR, data_req_data, sizeof(data_req_data), false, 2000);
  //CHECK(ret, sizeof(data_req_data), "req_state");
  if (ret != sizeof(data_req_data)) return ret;
  return 0;
}

int wii_i2c_read_state(i2c_inst_t *i2c, unsigned char *data)
{
  int ret = i2c_read_timeout_us(i2c, WII_I2C_ADDR, data, 6, false, 2000);
  //CHECK(ret, 6, "read_state");
  if (ret != sizeof(data_req_data)) return ret;
  return 0;
}

void wii_i2c_decode_nunchuk(const unsigned char *data, struct wii_i2c_nunchuk_state *state)
{
  if (! data) {
    memset(state, 0, sizeof(*state));
    return;
  }

  state->x = data[0] - (1<<7);
  state->y = data[1] - (1<<7);
  state->acc_x = ((data[2] << 2) | ((data[5] & 0x0c) >> 2)) - (1<<9);
  state->acc_y = ((data[3] << 2) | ((data[5] & 0x30) >> 4)) - (1<<9);
  state->acc_z = ((data[4] << 2) | ((data[5] & 0xc0) >> 6)) - (1<<9);
  state->c = (data[5] & 0x02) ? 0 : 1;
  state->z = (data[5] & 0x01) ? 0 : 1;
}

void wii_i2c_decode_classic(const unsigned char *data, struct wii_i2c_classic_state *state)
{
  if (! data) {
    memset(state, 0, sizeof(*state));
    return;
  }
  
  state->lx = (data[0] & 0x3f) - (1<<5);
  state->ly = (data[1] & 0x3f) - (1<<5);
  state->rx = (((data[0] & 0xc0) >> 3) | ((data[1] & 0xc0) >> 5) | (data[2] >> 7)) - (1<<4);
  state->ry = (data[2] & 0x1f) - (1<<4);
  
  state->a_lt = ((data[2] & 0x60) >> 2) | ((data[3] & 0xe0) >> 5);
  state->a_rt = data[3] & 0x1f;
  state->d_lt = (data[4] & 0x20) ? 0 : 1;
  state->d_rt = (data[4] & 0x02) ? 0 : 1;
  
  state->left  = (data[5] & 0x02) ? 0 : 1;
  state->right = (data[4] & 0x80) ? 0 : 1;
  state->up    = (data[5] & 0x01) ? 0 : 1;
  state->down  = (data[4] & 0x40) ? 0 : 1;
  state->a     = (data[5] & 0x10) ? 0 : 1;
  state->b     = (data[5] & 0x40) ? 0 : 1;
  state->x     = (data[5] & 0x08) ? 0 : 1;
  state->y     = (data[5] & 0x20) ? 0 : 1;
  state->plus  = (data[4] & 0x04) ? 0 : 1;
  state->minus = (data[4] & 0x10) ? 0 : 1;
  state->home  = (data[4] & 0x08) ? 0 : 1;
  state->zl    = (data[5] & 0x80) ? 0 : 1;
  state->zr    = (data[5] & 0x04) ? 0 : 1;
}
