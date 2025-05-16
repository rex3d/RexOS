[BITS 16]
[ORG 0x7C00]

extern gdt_descriptor
extern print_string
extern wait_key

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov si, msg1
    call print_string
    call wait_key

    ; Załaduj GDT
    lgdt [gdt_descriptor]

    ; Włącz PE bit (protected mode)
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Skok daleki do kodu 32-bitowego
    jmp 0x08:protected_mode_entry

[BITS 32]
protected_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Tu można odpalać dalszy kod, np. kernel albo long mode
    jmp $

section .data
msg1 db 'BOOTLOADER DZIALA! Nacisnij klawisz...', 0

; Padding
times 510 - ($ - $$) db 0
dw 0xAA55
