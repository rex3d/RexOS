ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000;

    .text ALIGN(0x1000) : {
        *(.text*)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata*)
    }

    .data ALIGN(0x1000) : {
        *(.data*)
    }

    .bss ALIGN(0x1000) (NOLOAD) : {
        *(.bss*)
    }

    /* Dodaj stack, kurwa, bo inaczej jebie się po chwili */
    .stack (NOLOAD) : {
        . = ALIGN(16);
        stack_start = .;
        . += 0x8000; /* 32KB stack */
        stack_end = .;
    }
}
