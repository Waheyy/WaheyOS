#include "print.h"
#include "framebuffer.h"
#include "waheylib.h"
#include <stdarg.h>
#include <stdint.h>

#define MAXBUF 0x100
Stream stream = {0};

int print_stream(Stream *stream) {

  stream->buf[stream->buf_len - 1] = '\0';
  Coords p =
      draw_string(stream->coords.x, stream->coords.y, stream->buf, 0x0f, 2);
  stream->coords.x = p.x;
  stream->coords.y = p.y;
  if (p.count != strlen(stream->buf)) {
    return 1;
  }
  memset(stream->buf, 0, stream->buf_len);

  return 0;
}

int push_to_buf(Stream *stream, char c) {
  stream->buf[stream->buf_idx] = c;
  stream->buf_idx++;

  if (stream->buf_idx >= stream->buf_len) {
    stream->buf_idx = 0;
    int err = print_stream(stream);
    if (err) {
      return err;
    }
  }

  return 0;
}

int push_all_to_buf(Stream *stream, char *str) {
  int err;
  while (*str) {
    if (*str == '\0')
      break;
    err = push_to_buf(stream, *str);
  }
  if (err) {
    return err;
  }
  return 0;
}

void push_int_to_buf(Stream *stream, unsigned int val, unsigned int base) {
  int i = MAXBUF - 2;
  static char buf[MAXBUF] = {0};
  for (; val && i; --i, val /= base) {
    buf[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[val % base];
  }
  push_all_to_buf(stream, &buf[i + 1]);
}

int base_from_flag(char flag) {
  switch (flag) {
  case 'd':
    return 10;
  case 'x':
    return 16;
  case 'o':
    return 8;
  default:
    return 10;
  }
}

void vfprintf(Stream *stream, const char *fmt, va_list args) {
  int base;

  for (size_t i = 0; fmt[i] != '\0'; i++) {
    char current = fmt[i];
    if (current != '%') {
      push_to_buf(stream, current);
      continue;
    }
    char specifier = fmt[++i];

    if (!specifier)
      break;
    switch (specifier) {
    case 'd':
    case 'o':
    case 'x': {
      base = base_from_flag(specifier);
      int val = va_arg(args, int);
      push_int_to_buf(stream, (unsigned int)val, base);
      break;
    }
    default: {
      continue;
    }
    }
  }
}

void printf(const char *fmt, ...) {
  static char buf[MAXBUF];
  static Stream stream = {
      .buf_len = MAXBUF, .buf_idx = 0, .coords = {0, 0, 0}, .buf = buf};

  va_list args;
  va_start(args, fmt);
  vfprintf(&stream, fmt, args);
  va_end(args);
}
