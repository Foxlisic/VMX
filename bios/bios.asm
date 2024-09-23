        org     0
; ----------------------------------------------------------------------
KEYBUF      EQU     0x5B00
KEYBFIFO    EQU     0x5B08
TMP16       EQU     0x5B10
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

        ei
        jr      $

        include "keyboard.asm"
        include "display.asm"
