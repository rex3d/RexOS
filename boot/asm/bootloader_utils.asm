[BITS 16]
global print_string
global wait_key

print_string:
    pusha
.next_char:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .next_char
.done:
    popa
    ret

wait_key:
    mov ah, 0
    int 0x16
    ret
