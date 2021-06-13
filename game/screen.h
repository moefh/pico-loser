
#ifndef SCREEN_H_FILE
#define SCREEN_H_FILE

#include "joystick.h"

int screen_init(int vga_pin_base);
void screen_render(struct JOYSTICK *joy);

#endif /* SCREEN_H_FILE */
