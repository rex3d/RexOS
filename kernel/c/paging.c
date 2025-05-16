#include "paging.h"
#include "utils.h"

#define PAGE_SIZE 4096
#define PAGE_DIR_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024

// Dla 32-bit
uint32_t page_directory[PAGE_DIR_ENTRIES] __attribute__((aligned(4096)));
uint32_t first_page_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(4096)));

// Dla 64-bit (jeśli chcesz, możesz tu dodać struktury dla PML4, PDPT itd.)

extern void enable_long_mode(); // z long_mode.asm

void paging_init(cpu_mode_t mode) {
    switch (mode) {
        case MODE_16BIT:
            // Kurwa, w 16-bit nie ma pagingu, jebnij to do kosza
            break;

        case MODE_32BIT:
            // Inicjalizacja prostego pagingu 32-bitowego
            for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
                first_page_table[i] = (i * PAGE_SIZE) | 3; // Present + RW
            }
            for (int i = 0; i < PAGE_DIR_ENTRIES; i++) {
                page_directory[i] = 0;
            }
            page_directory[0] = ((uint32_t)first_page_table) | 3;

            asm volatile(
                "mov %0, %%cr3\n"
                "mov %%cr0, %%eax\n"
                "orl $0x80000000, %%eax\n"
                "mov %%eax, %%cr0"
                :
                : "r"(page_directory)
                : "eax"
            );
            break;

        case MODE_64BIT:
            // W 64-bit przerzucamy na long_mode.asm, który ustawia paging i włącza long mode
            enable_long_mode();
            break;
    }
}
