[BITS 16]
section .data
global gdt_descriptor

gdt_start:
    dq 0x0000000000000000         ; null
    dq 0x00CF9A000000FFFF         ; code segment (selector = 0x08)
    dq 0x00CF92000000FFFF         ; data segment (selector = 0x10)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
