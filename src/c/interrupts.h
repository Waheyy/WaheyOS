#ifndef INTERRUPT_H
#define INTERRUPT_H

void install_vector_table(void);
void irq_handler(void);
void interrupt_init(void);
void enable_irq(void);

#endif
