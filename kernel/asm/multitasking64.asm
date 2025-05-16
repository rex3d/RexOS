global switch_task64_asm

section .text

; void switch_task64_asm(uint64_t* old_rsp, uint64_t* old_rbp, uint64_t new_rsp, uint64_t new_rbp);
; argumenty (System V AMD64 ABI):
; rdi = old_rsp, rsi = old_rbp, rdx = new_rsp, rcx = new_rbp

switch_task64_asm:
    mov [rdi], rsp
    mov [rsi], rbp
    mov rsp, rdx
    mov rbp, rcx
    ret
