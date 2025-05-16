ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000; /* kernel ładowany pod 1MB */

    .text ALIGN(0x1000) : {
        *(.text)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata)
    }

    .data ALIGN(0x1000) : {
        *(.data)
    }

    .bss ALIGN(0x1000) : {
        *(.bss)
        *(COMMON)
    }
    
    /* Opcjonalnie, możesz dodać tu sekcję dla stosu, jeśli potrzebujesz */
    /*
    .stack (NOLOAD) : {
        . = ALIGN(16);
        stack_start = .;
        . += 0x4000; // 16KB stosu
        stack_end = .;
    }
    */
}
