global switch_task32_asm

section .text

; void switch_task32_asm(uint32_t* old_esp, uint32_t* old_ebp, uint32_t new_esp, uint32_t new_ebp)
; argumenty w rejestrach lub na stosie - zależy od ABI i kompilatora

switch_task32_asm:
    mov [esp + 4], esp    ; zapisz ESP (może wymagać poprawek)
    mov [esp + 8], ebp    ; zapisz EBP
    mov esp, [esp + 12]   ; nowy ESP
    mov ebp, [esp + 16]   ; nowy EBP
    ret
