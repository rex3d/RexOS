; read_sector_16.asm
; Wejście:
;   BX - offset bufora w segmencie ES
;   ES - segment bufora
;   DL - numer dysku (0x80 dla HDD)
;   CX - cylinder/sektor
;   DH - głowica
;   AL - ilość sektorów (zwykle 1)
; Wyjście:
;   CF ustawiony jeśli błąd
;   AH - kod błędu

global read_sector_16

section .text
read_sector_16:
    push ax
    push bx
    push cx
    push dx
    push es

    mov ah, 0x02    ; funkcja BIOS - read sectors
    int 0x13
    jc error

    pop es
    pop dx
    pop cx
    pop bx
    pop ax
    ret

error:
    pop es
    pop dx
    pop cx
    pop bx
    pop ax
    stc             ; ustaw carry flag (błąd)
    ret
