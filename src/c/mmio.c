#include "mmio.h"
#include <stdint.h>

void mmio_write(uintptr_t reg, uint32_t value) {
  *(volatile uint32_t *)reg = value;
}

uint32_t mmio_read(uintptr_t reg) { return *(volatile uint32_t *)reg; }
