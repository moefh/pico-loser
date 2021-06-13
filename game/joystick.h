#ifndef JOYSTICK_H_FILE
#define JOYSTICK_H_FILE

#define JOY_BTN_UP     (1u<<0)
#define JOY_BTN_DOWN   (1u<<1)
#define JOY_BTN_LEFT   (1u<<2)
#define JOY_BTN_RIGHT  (1u<<3)
#define JOY_BTN_A      (1u<<4)
#define JOY_BTN_B      (1u<<5)
#define JOY_BTN_C      (1u<<6)
#define JOY_BTN_D      (1u<<7)
#define JOY_BTN_E      (1u<<8)
#define JOY_BTN_F      (1u<<9)

#define JOY_BTN_PRESSED(joy, btn)  (((joy)->cur  & (btn)) && ! ((joy)->last & (btn)))
#define JOY_BTN_RELEASED(joy, btn) (((joy)->last & (btn)) && ! ((joy)->cur  & (btn)))

struct JOYSTICK {
  void *hw;
  const char *name;
  int enabled;
  unsigned int last;
  unsigned int cur;
};

#endif /* JOYSTICK_H_FILE */
