; ----------------------------------------------------------------------
; Вывод информации на дисплей
; ----------------------------------------------------------------------

; Очистить экран в цвет A :: 38B (11B + 27B)
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
