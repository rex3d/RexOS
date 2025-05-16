#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void interrupts_init();
void isr_handler();
void irq_handler();

#endif
