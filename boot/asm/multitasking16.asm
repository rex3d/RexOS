global switch_task16_asm

section .text

; void switch_task16_asm(uint16_t* old_sp, uint16_t* old_bp, uint16_t new_sp, uint16_t new_bp)
; argumenty na stosie (bo 16-bit, fajrant)

switch_task16_asm:
    pusha
    mov ax, [sp + 14]  ; wskaźnik old_sp
    mov bx, [sp + 16]  ; wskaźnik old_bp
    mov cx, [sp + 18]  ; new_sp
    mov dx, [sp + 20]  ; new_bp

    mov [ax], sp       ; zapisz stary SP
    mov [bx], bp       ; zapisz stary BP

    mov sp, cx         ; ustaw nowy SP
    mov bp, dx         ; ustaw nowy BP

    popa
    ret
