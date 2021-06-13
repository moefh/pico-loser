
#ifndef JOY_WII_I2C_H_FILE
#define JOY_WII_I2C_H_FILE

#include "joystick.h"

int joy_wii_i2c_init(struct JOYSTICK *joy, unsigned int sda_pin, unsigned int scl_pin);
void joy_wii_i2c_request_state(struct JOYSTICK *joy);
void joy_wii_i2c_read_state(struct JOYSTICK *joy);
void joy_wii_i2c_start_auto_update(struct JOYSTICK *joy);
void joy_wii_i2c_update(struct JOYSTICK *joy);

#endif /* JOY_WII_I2C_H_FILE */
