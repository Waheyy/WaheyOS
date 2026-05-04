#include "uart0.h"

void kernel_entry() {
  uart0_init();
  writeText("WAHEY OPERATING SYSTEM FOR THE RASPI\n");

  while (1) {
    uart_update();
  }
}
