#ifndef PRINT_H
#define PRINT_H
#include "framebuffer.h"

typedef struct Stream {
  size_t buf_len;
  size_t buf_idx;
  Coords coords;
  char *buf;
} Stream;

void printf(const char *fmt, ...);
#endif
