#ifndef WII_I2C_H_FILE
#define WII_I2C_H_FILE

#include "hardware/i2c.h"

#define WII_I2C_IDENT_NONE     0
#define WII_I2C_IDENT_NUNCHUK  0xa4200000
#define WII_I2C_IDENT_CLASSIC  0xa4200101

struct wii_i2c_nunchuk_state {
  // accelerometer
  short int acc_x;
  short int acc_y;
  short int acc_z;

  // analog stick:
  signed char x;
  signed char y;

  // buttons:
  char c;
  char z;
};

struct wii_i2c_classic_state {
  // analog sticks:
  signed char lx;
  signed char ly;
  signed char rx;
  signed char ry;

  // triggers (a_ is the analog part, d_ is the click bit):
  unsigned char a_lt;
  unsigned char a_rt;
  char d_lt;
  char d_rt;

  // d-pad:
  char up;
  char down;
  char left;
  char right;

  // buttons:
  char a;
  char b;
  char x;
  char y;

  // bumpers:
  char zr;
  char zl;

  // face buttons:
  char home;
  char plus;
  char minus;
};

int wii_i2c_init(i2c_inst_t *i2c, unsigned int sda_pin, unsigned int scl_pin);
int wii_i2c_read_ident(i2c_inst_t *i2c, unsigned int *ident);
int wii_i2c_request_state(i2c_inst_t *i2c);
int wii_i2c_read_state(i2c_inst_t *i2c, unsigned char *data);

void wii_i2c_decode_nunchuk(const unsigned char *data, struct wii_i2c_nunchuk_state *state);
void wii_i2c_decode_classic(const unsigned char *data, struct wii_i2c_classic_state *state);

#endif /* WII_I2C_H_FILE */
