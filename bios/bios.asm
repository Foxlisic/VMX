        org     0
; ----------------------------------------------------------------------
KEYBUF      EQU     0x5B00  ; Адрес буфера состояния клавиш
KEYBFIFO    EQU     0x5B08  ; Клавиатурный буфер FIFO
TMP16       EQU     0x5B10  ; Временное 16-битное значение
LOCYX       EQU     0x5B12  ; Текущее положение курсора
CURBLINK    EQU     0x5B14  ; 0..24 Курсора нет 25..49 Курсор проявлен
CURFORM     EQU     0x0F    ; Форма курсора
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
        call    cursor_blink_irq
        pop     bc
        pop     de
        pop     hl
        pop     af
        ei
        reti

; ---------------------------------------------------------------------
reset:  im      0
        ld      a, (0*8) + 4
        call    cls
        call    kbd_init
        ei

        ld      hl, $1700
        call    locate

.x:     call    kbd_fetch
        jr      z, .x
        call    prn
        jr      .x

        include "keyboard.asm"
        include "display.asm"
