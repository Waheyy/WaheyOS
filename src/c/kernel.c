#include "framebuffer.h"
#include "uart0.h"
unsigned int colpal[] = {0x000000, 0x0000AA, 0x00AA00, 0x00AAAA,
                         0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA,
                         0x555555, 0x5555FF, 0x55FF55, 0x55FFFF,
                         0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF};

void delay(volatile int count) {
  while (count--) {
    __asm__ volatile("nop");
  }
}

void kernel_entry() {
  uart0_init();
  framebuffer_init();

  while (1) {
    draw_rect(0, 0, 1920, 1080, 0, 1);
    for (int i = 1; i < 0x10; i++) {
      draw_string(460, 520, "WAHEYY OPERATING SYSTEM!!", (0 << 4 | i), 5);
      delay(3000000);
    }
  }
}
