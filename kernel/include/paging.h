#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

typedef enum {
    MODE_16BIT,
    MODE_32BIT,
    MODE_64BIT
} cpu_mode_t;

// Inicjalizacja pagingu dla podanego trybu CPU.
// Dla 16-bit: nic nie robi (brak pagingu).
// Dla 32-bit: ustawia prosty page directory + page table na 4MB.
// Dla 64-bit: ustawia PML4, PDPT, itd. zgodnie z long_mode.asm.
void paging_init(cpu_mode_t mode);

#endif
