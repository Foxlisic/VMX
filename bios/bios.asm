        org     0
; ----------------------------------------------------------------------
KEYBUF      EQU     0x5B00  ; Адрес буфера состояния клавиш
KEYBFIFO    EQU     0x5B08  ; Клавиатурный буфер FIFO
TMP16       EQU     0x5B10  ; Временное 16-битное значение
LOCYX       EQU     0x5B12  ; Текущее положение курсора
; ---------------------------------------------------------------------
rst00:  di                  ; 1
        xor     a           ; 1
        ld      sp, 0       ; 3
        jp      reset       ; 3
        defb    0,0,0,0,0,0,0,0 ; #08 RST
        defb    0,0,0,0,0,0,0,0 ; #10 RST
        defb    0,0,0,0,0,0,0,0 ; #18 RST
        defb    0,0,0,0,0,0,0,0 ; #20 RST
        defb    0,0,0,0,0,0,0,0 ; #28 RST
        defb    0,0,0,0,0,0,0,0 ; #30 RST

; Прерывание 50 ГЦ (или 60 Гц)
rst38:  push    af
        push    hl
        push    de
        push    bc
        call    kbd_irq
        pop     bc
        pop     de
        pop     hl
        pop     af
        ei
        reti

; ---------------------------------------------------------------------
reset:  ld      a, $0F
        call    cls
        call    kbd_init

        ld      hl, $0000
        call    locate

.a1:    ld      a, 'x'
        call    pchr
        ld      a,(LOCYX)
        inc     a
        ld      (LOCYX),a
        cp      $40
        jr      nz, .a1
        xor     a
        ld      (LOCYX),a
        ld      a,(LOCYX+1)
        inc     a
        ld      (LOCYX+1),a
        cp      24
        jr      nz, .a1

        ei
        jr      $

        include "keyboard.asm"
        include "display.asm"
