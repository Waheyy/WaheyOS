#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <stdint.h>

typedef struct Coords {
  size_t x;
  size_t y;
  size_t count; // optional to capture processed characters
} Coords;

void framebuffer_init();

void draw_pixel(int x, int y, uint8_t attr);

void draw_rect(int x1, int y1, int x2, int y2, uint8_t attr, int fill);

void draw_line(int x1, int y1, int x2, int y2, uint8_t attr);

void draw_circle(int x0, int y0, int radius, uint8_t attr, int fill);

void draw_char(unsigned char ch, int x, int y, uint8_t attr, int scale);

Coords draw_string(int x, int y, char *s, uint8_t attr, int scale);

void draw_cell_rect(int gx, int gy, uint8_t attr, int fill);
#endif
