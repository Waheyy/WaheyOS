#include <stdint.h>

void *memset(void *dest_ptr, int x, size_t n) {
  unsigned char *p = dest_ptr;
  while (n--) {
    *p++ = (unsigned char)x;
  }
  return dest_ptr
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (*str++) {
    len++
  }
  return len;
}
