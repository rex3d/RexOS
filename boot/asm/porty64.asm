global outb64
global inb64

section .text

; void outb64(uint16_t port, uint8_t val)
; argumenty w rejestrach (System V AMD64 ABI):
; port -> di, val -> sil
outb64:
    mov dx, di
    mov al, sil
    out dx, al
    ret

; uint8_t inb64(uint16_t port)
; argumenty w di
inb64:
    mov dx, di
    in al, dx
    movzx eax, al
    ret
