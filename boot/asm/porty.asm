global outb
global inb

section .text

; void outb(uint16_t port, uint8_t val)
; argumenty na stosie (32-bit)
outb:
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

; uint8_t inb(uint16_t port)
; argumenty na stosie (32-bit)
inb:
    mov dx, [esp + 4]
    in al, dx
    movzx eax, al
    ret
