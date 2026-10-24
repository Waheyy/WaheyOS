#include "framebuffer.h"
#include "terminal.h"
#include "vcmbox.h"
#include <stdint.h>

uint32_t width, height, pitch, isrgb;
unsigned char *fb;

void framebuffer_init() {     // makes message to get a framebuffer
  mbox_msg[0] = 35 * 4;       // size in bytes
  mbox_msg[1] = MBOX_REQUEST; // 0 since this is request

  // tags start here
  mbox_msg[2] = TAG_SET_PHYWH;
  mbox_msg[3] = 8;
  mbox_msg[4] = MBOX_REQUEST;
  mbox_msg[5] = 1920;
  mbox_msg[6] = 1080;

  mbox_msg[7] = TAG_SET_VIRTWH;
  mbox_msg[8] = 8;
  mbox_msg[9] = MBOX_REQUEST;
  mbox_msg[10] = 1920;
  mbox_msg[11] = 1080;

  mbox_msg[12] = TAG_SET_VIRTOFFSET;
  mbox_msg[13] = 8;
  mbox_msg[14] = MBOX_REQUEST;
  mbox_msg[15] = 0; // value(x)
  mbox_msg[16] = 0; // value(y)

  mbox_msg[17] = TAG_SET_DEPTH;
  mbox_msg[18] = 4;
  mbox_msg[19] = MBOX_REQUEST;
  mbox_msg[20] = 32; // bits per pixel argb so each letter is 1 byte

  mbox_msg[21] = TAG_SET_PXLORD;
  mbox_msg[22] = 4;
  mbox_msg[23] = MBOX_REQUEST;
  mbox_msg[24] = 1;

  mbox_msg[25] = TAG_GET_FB;
  mbox_msg[26] = 8;
  mbox_msg[27] = MBOX_REQUEST;
  mbox_msg[28] = 4096;
  mbox_msg[29] = 0;

  mbox_msg[30] = TAG_GET_PITCH;
  mbox_msg[31] = 4;
  mbox_msg[32] = MBOX_REQUEST;
  mbox_msg[33] = 0;

  mbox_msg[34] = MBOX_ENDTAG;

  // makes the request and makes sure that a pointer is returned to fb with 32
  // depth
  if (mbox_call(CHANNEL_ARMTOVC) && mbox_msg[20] == 32 && mbox_msg[28] != 0) {
    mbox_msg[28] &= 0x3fffffff; // masks away the top 2 bits
    width = mbox_msg[10];
    height = mbox_msg[11];
    pitch = mbox_msg[33];
    isrgb = mbox_msg[24];
    fb = (unsigned char *)((uintptr_t)mbox_msg[28]);
  }
}

void draw_pixel(int x, int y, uint8_t attr) {
  // finds offset within framebuffer memory array by multiplying y value uw with
  // the pitch to find the right byte moves down y rows pitch is number of bytes
  // per row multiply x by 4 to find byte position in x axis (row)
  int offset = (y * pitch) + (x * 4);
  // masks and keeps only lower 4 bits which is what i want
  // dereference the memory address and write the color to it
  *((uint32_t *)(fb + offset)) = vgapal[attr & 0x0f];
}

// i do not enjoy dsa but basically goes to every single pixel in the rectangle
// if no fill then everytime pixel is an outline pixel draw a pixel
void draw_rect(int x1, int y1, int x2, int y2, uint8_t attr, int fill) {
  int y = y1;

  while (y <= y2) {
    int x = x1;

    while (x <= x2) {
      if ((x == x1 || x == x2) || (y == y1 || y == y2)) {
        draw_pixel(x, y, attr);
      } else if (fill) {
        draw_pixel(x, y, (attr & 0xf0) >> 4);
      }
      x++;
    }
    y++;
  }
}

// i hope the ppl reading my code understand that i suck at math and do not wish
// to get this bresenham line alg
void draw_line(int x1, int y1, int x2, int y2, uint8_t attr) {
  int dx, dy, p, x, y;

  dx = x2 - x1;
  dy = y2 - y1;
  x = x1;
  y = y1;
  p = 2 * dy - dx;

  while (x < x2) {
    if (p >= 0) {
      draw_pixel(x, y, attr);
      y++;
      p = p + 2 * dy - 2 * dx;
    } else {
      draw_pixel(x, y, attr);
      p = p + 2 * dy;
    }
    x++;
  }
}

// nah i give up i am just copy and pasting now what is this even bro
void draw_circle(int x0, int y0, int radius, uint8_t attr, int fill) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    if (fill) {
      draw_line(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
      draw_line(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
      draw_line(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
      draw_line(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
    }
    draw_pixel(x0 - y, y0 + x, attr);
    draw_pixel(x0 + y, y0 + x, attr);
    draw_pixel(x0 - x, y0 + y, attr);
    draw_pixel(x0 + x, y0 + y, attr);
    draw_pixel(x0 - x, y0 - y, attr);
    draw_pixel(x0 + x, y0 - y, attr);
    draw_pixel(x0 - y, y0 - x, attr);
    draw_pixel(x0 + y, y0 - x, attr);

    if (err <= 0) {
      y += 1;
      err += 2 * y + 1;
    }

    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

// pray to god this works my math is too bad
void draw_char(unsigned char ch, int x, int y, uint8_t attr, int scale) {
  unsigned char *glyph =
      (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

  if (!scale) {
    scale = 1;
  }
  for (int i = 0; i < FONT_HEIGHT; i++) {
    for (int j = 0; j < FONT_WIDTH; j++) {
      unsigned char mask = 1 << j;
      unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;
      // draw_pixel(x + j, y + i, col);
      for (int dy = 0; dy < scale; dy++) {
        for (int dx = 0; dx < scale; dx++) {
          draw_pixel(x + j * scale + dx, y + i * scale + dy, col);
        };
      }
    }
    glyph += FONT_BPL;
  }
}

Coords draw_string(int x, int y, char *s, uint8_t attr, int scale) {

  if (x < 0 || x > +1920 || y < 0 || y >= 1080) {
    Coords p = {.x = x, .y = y};
    return p;
  }
  int count = 0;
  while (*s) {
    if (*s == '\r') {
      x = 0;
    } else if (*s == '\n') {
      x = 0;
      y += FONT_HEIGHT * scale;
    } else {
      draw_char(*s, x, y, attr, scale);
      x += FONT_WIDTH * scale;
    }
    s++;
    count++;
  }
  Coords p = {.x = x, .y = y, .count = count};
  return p;
}

// put pixels into grids to fill whole cells
void draw_cell_rect(int gx, int gy, uint8_t attr, int fill) {
  int x1 = gx * FONT_WIDTH;
  int y1 = gy * FONT_HEIGHT;

  int x2 = x1 + FONT_WIDTH - 1;
  int y2 = y1 + FONT_HEIGHT - 1;

  draw_rect(x1, y1, x2, y2, attr, fill);
}
