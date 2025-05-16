; bootloader_pm.asm - przejście do protected mode z real mode
; Ładuje kernel (32-bit) i przekazuje mu kontrolę

[BITS 16]
[ORG 0x7C00]

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

    ; Załaduj kernel (16 sektorów -> 0x100000)
    mov ah, 0x02
    mov al, 16          ; liczba sektorów
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    mov bx, 0x0000
    mov es, 0x1000      ; 0x100000 = 0x1000:0x0000
    int 0x13
    jc disk_error

    call setup_gdt

    ; Włącz tryb chroniony
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 08h:pm_start

; ===============================
; ========== PROTECTED ==========
; ===============================
[BITS 32]
pm_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov esi, msg2
    call print_string_pm

    jmp 0x100000      ; Skok do kernela

; ===============================
; ========= ERROR MSG ===========
; ===============================
[BITS 16]
disk_error:
    mov si, disk_err_msg
    call print_string
    mov ah, 0x0E
    mov al, 7
    int 0x10

.halt:
    hlt
    jmp .halt

; ===============================
; ========= FUNCTIONS ===========
; ===============================
print_string:
    lodsb
    cmp al, '$'
    je .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

[BITS 32]
print_string_pm:
    lodsb
    cmp al, '$'
    je .done_pm
    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    jmp print_string_pm
.done_pm:
    ret

[BITS 16]
setup_gdt:
    lgdt [gdt_descriptor]
    ret

; ===============================
; ========= STRINGS =============
; ===============================
msg1 db 'Przejscie do protected mode...', '$'
msg2 db 'Jestes w protected mode!', '$'
disk_err_msg db 'Blad odczytu dysku!$', 0

; ===============================
; ========== GDT ================
; ===============================
gdt_start:
    dq 0x0000000000000000         ; Null
    dq 0x00CF9A000000FFFF         ; Code 0x08
    dq 0x00CF92000000FFFF         ; Data 0x10
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Boot sector padding (musi być dokładnie 512 bajtów)
times 510 - ($ - $$) db 0
dw 0xAA55
