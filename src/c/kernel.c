#include "framebuffer.h"
#include "interrupts.h"
#include "print.h"
#include "uart0.h"

void delay(volatile int count) {
  while (count--) {
    __asm__ volatile("nop");
  }
}

void kernel_entry() {
  uart0_init();
  framebuffer_init();
  interrupt_init();
  enable_irq();

  // while (1) {
  //   draw_rect(0, 0, 1920, 1080, 0, 1);
  //   for (int i = 1; i < 0x10; i++) {
  //     draw_string(460, 520, "WAHEYY OPERATING SYSTEM!!", (0 << 4 | i), 5);
  //     delay(3000000);
  //   }
  // }

  while (1) {
    printf("WAHEY OPERATING SYSTEM %d\n", 15);
    printf("WAHEY OPERATING SYSTEM %d\n", 16);
    printf("WAHEY OPERATING SYSTEM %d\n", 17);
    printf("WAHEY OPERATING SYSTEM %d\n", 18);
    printf("WAHEY OPERATING SYSTEM %d\n", 19);
  }
}
