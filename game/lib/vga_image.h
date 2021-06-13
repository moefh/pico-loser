#ifndef VGA_IMAGE_H_FILE
#define VGA_IMAGE_H_FILE

#include <stdbool.h>

#include "vga_6bit.h"

#ifdef __cplusplus
extern "C" {
#endif

struct VGA_IMAGE {
  int width;
  int height;
  unsigned int stride;  // number of words per line
  const unsigned int *data;
};

void vga_image_draw(const struct VGA_IMAGE *image, int x, int y, bool transparent);
void vga_image_draw_frame(const struct VGA_IMAGE *img, int frame, int x, int y, bool transparent);

#ifdef __cplusplus
}
#endif

#endif /* DRAW_H_FILE */
