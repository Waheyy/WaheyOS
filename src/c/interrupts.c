#include "interrupts.h"
#include "framebuffer.h"
#include <stdint.h>

void interrupt_init() { install_vector_table(); }

void enable_irq() { asm volatile("msr daifclr, #2"); }

void irq_handler() { draw_string(460, 520, "INTERRUPT IS HANDLED", 10, 5); }
