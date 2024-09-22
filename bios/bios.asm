; ----------------------------------------------------------------------
KEYBUF      EQU     0x5B00
KEYBFIFO    EQU     0x5B08
KEYBCNT     EQU     0x5B0F
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
rst38:  halt
        push    af
        push    hl
        push    de
        push    bc

        ; Цикл проверки изменений
        ld      hl, KEYBUF
        ld      b, $FE
.r1:    ld      c, $FE
        in      a, (c)
        cp      (hl)
        jr      z, .sk1
        ; --- есть изменения в нажатии ---
.sk1:   inc     hl
        rlc     b
        jr      c, .r1

        pop     bc
        pop     de
        pop     hl
        pop     af
        ei
        reti

reset:  ld      a, $0F
        call    cls

        ; Очистка клавиатурного буфера
        ld      hl, KEYBUF
        ld      bc, 0x08FF
.kbclr: ld      (hl), c
        inc     hl
        djnz    .kbclr
        ei
        jr      $

; ----------------------------------------------------------------------
; Очистить экран :: 38B (11B + 27B)
; ----------------------------------------------------------------------

cls:    ld      b, a
        rrca
        rrca
        rrca
        out     ($FE),a
        ld      a, b
        ld      hl, $0000       ; 3
        add     hl, sp          ; 1
        ld      (TMP16), hl     ; 3
        ld      hl, $0000       ; 3
        ld      sp, $5B00       ; 3
        ld      bc, $6003       ; 3 :: 96 на атрибуты, 3*256 на экран
        ld      d, a            ; 1
        ld      e, a            ; 1
.a:     push    de  ; 1
        push    de  ; 1
        push    de  ; 1
        push    de  ; 1
        djnz    .a  ; 2
.b:     push    hl  ; 1
        push    hl  ; 1
        push    hl  ; 1
        push    hl  ; 1
        djnz    .b  ; 2
        dec     c   ; 1
        jr      nz, .b      ; 2
        ld      hl, (TMP16) ; 3
        ld      sp, hl      ; 1
        ret                 ; 1
