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
        ld      hl, $0000
        add     hl, sp
        ld      (TMP16), hl
        ld      hl, $0000
        ld      sp, $5B00
        ld      bc, $6003       ; 96 на атрибуты, 3*256 на экран
        ld      d, a
        ld      e, a
.a:     push    de
        push    de
        push    de
        push    de
        djnz    .a
.b:     push    hl
        push    hl
        push    hl
        push    hl
        djnz    .b
        dec     c
        jr      nz, .b
        ld      hl, (TMP16)
        ld      sp, hl
        ld      hl, $0000       ; Инициализировать курсор
        call    locate
        ret

; ----------------------------------------------------------------------
; Вычислить HL по координатам H=Y L=X
; ----------------------------------------------------------------------
; H ...bb000
; L aaaxxxxx
; Y    bbaaa

ploc:   ld      a, l
        and     a, $3E          ; L[5:1] -> L[4:0]
        rrca
        ld      l, a
        ld      a, h
        rrca
        rrca
        rrca
        and     a, $E0          ; H[2:0] -> L[7:5]
        or      a, l
        ld      l, a            ; L=aaaxxxxx
        ld      a, h
        and     a, $18
        add     a, $40
        ld      h, a            ; H[2:0]=0, H[6]=1
        ret

; ----------------------------------------------------------------------
; Печать символа A в LOCYX=(Y,X)
; ----------------------------------------------------------------------

pchr:   sub     a, $20
        ld      de, fontrom     ; Вычислить начальную позицию
        ld      c, 0            ; C=0
        ld      h, c            ; H=0
        ld      l, a
        srl     l
        add     hl, hl
        add     hl, hl
        add     hl, hl
        add     hl, de
        ex      de, hl          ; DE = 8*((A - 0x20) >> 1) + fontrom
        rrca
        rl      c               ; C[1] Сдвиг символа вправо
        ld      a, (LOCYX)
        rrca
        rl      c               ; C[0] Нечетный столбец
        ld      hl, (LOCYX)
        call    ploc
        ld      b, 8            ; Высота символа 8 строк
.m4:    ld      a, (de)         ; Прочесть источник данных
        bit     1, c            ; Тест бита конфигурации
        jr      nz, .m1         ; Если там 1, то тогда знакоместо не двигать
        rrca                    ; Если 0, то сдвинуть знакоместо на 4 бита вправо
        rrca
        rrca
        rrca
.m1:    and     $0F             ; В любом случае срезать старшие 4 бита изображения
        push    bc
        bit     0, c
        jr      nz, .m2         ; Вывод символа слева или справа
        rlca                    ; Сдвинуть на 4 влево
        rlca
        rlca
        rlca
        ld      b, a
        ld      a, (hl)
        and     $0F             ; Срезать 4 пикселя (были) слева
        jr      .m3
.m2:    ld      b, a
        ld      a, (hl)
        and     $F0             ; Срезать 4 пикселя (были) справа
.m3:    or      b               ; Наложить пиксели
        ld      (hl), a         ; Запись обратно
        pop     bc
        inc     de
        inc     h
        djnz    .m4
        ret

; ----------------------------------------------------------------------
; Установка курсора H=Y L=X
; ----------------------------------------------------------------------

locate: di
        ld      (LOCYX), hl
        ; Если курсор в состоянии мигания
        ei
        ret

; ----------------------------------------------------------------------
fontrom:; Шрифты 4x8
        defb 0x00,0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00,0xAA,0xAE,0x0A,0x0A,0x0E,0x0A,0x00
        defb 0x40,0x4A,0xA2,0x44,0x24,0xA8,0x4A,0x40,0x40,0xA4,0x44,0xA0,0xA0,0xC0,0x60,0x00
        defb 0x00,0x28,0x44,0x44,0x44,0x44,0x28,0x00,0x00,0xA4,0x44,0xEE,0x44,0xA4,0x00,0x00
        defb 0x00,0x00,0x00,0x0E,0x00,0x40,0x40,0x80,0x00,0x02,0x02,0x04,0x04,0x48,0x48,0x00
        defb 0x00,0x44,0xAC,0xE4,0xA4,0xA4,0x4E,0x00,0x00,0x4E,0xA2,0x24,0x42,0x8A,0xE4,0x00
        defb 0x00,0x2E,0x68,0xAC,0xE2,0x2A,0x24,0x00,0x00,0x6E,0x82,0xC2,0xA4,0xA4,0x44,0x00
        defb 0x00,0x44,0xAA,0x4A,0xA6,0xA2,0x4C,0x00,0x00,0x00,0x44,0x00,0x00,0x44,0x08,0x00
        defb 0x00,0x00,0x2E,0x40,0x8E,0x40,0x20,0x00,0x00,0x04,0x8A,0x42,0x24,0x40,0x84,0x00
        defb 0x00,0x44,0xAA,0xAA,0x8E,0x8A,0x6A,0x00,0x00,0xC4,0xAA,0xC8,0xA8,0xAA,0xC4,0x00
        defb 0x00,0xCE,0xA8,0xAE,0xA8,0xA8,0xCE,0x00,0x00,0xE4,0x8A,0xE8,0x8A,0x8A,0x84,0x00
        defb 0x00,0xAE,0xA4,0xE4,0xA4,0xA4,0xAE,0x00,0x00,0x2A,0x2A,0x2C,0x2A,0xAA,0x4A,0x00
        defb 0x00,0x8A,0x8E,0x8A,0x8A,0x8A,0xEA,0x00,0x00,0x24,0xAA,0xEA,0xEA,0xAA,0x84,0x00
        defb 0x00,0xC4,0xAA,0xAA,0xCA,0x8C,0x86,0x00,0x00,0xC4,0xAA,0xA4,0xC2,0xAA,0xA4,0x00
        defb 0x00,0xEA,0x4A,0x4A,0x4A,0x4A,0x4E,0x00,0x00,0xAA,0xAA,0xAA,0xAA,0xAE,0x4A,0x00
        defb 0x00,0xAA,0xAA,0x44,0xA4,0xA4,0xA4,0x00,0x00,0xE6,0x24,0x44,0x44,0x84,0xE6,0x00
        defb 0x00,0x8C,0x84,0x44,0x44,0x24,0x2C,0x00,0x00,0x40,0xA0,0x00,0x00,0x00,0x00,0x0F
        defb 0x00,0x80,0x4C,0x02,0x06,0x0A,0x06,0x00,0x00,0x80,0x86,0xC8,0xA8,0xA8,0xC6,0x00
        defb 0x00,0x20,0x24,0x6A,0xAE,0xA8,0x66,0x00,0x00,0x20,0x46,0xEA,0x4A,0x46,0x42,0x0C
        defb 0x00,0x84,0x80,0xCC,0xA4,0xA4,0xAE,0x00,0x00,0x28,0x08,0x6A,0x2C,0x2A,0x2A,0xC0
        defb 0x00,0x40,0x4A,0x4E,0x4A,0x4A,0x4A,0x00,0x00,0x00,0xC4,0xAA,0xAA,0xAA,0xA4,0x00
        defb 0x00,0x00,0xC6,0xAA,0xAA,0xC6,0x82,0x82,0x00,0x00,0x66,0x88,0x84,0x82,0x8C,0x00
        defb 0x00,0x40,0x4A,0xEA,0x4A,0x4A,0x4E,0x00,0x00,0x00,0xAA,0xAA,0xAA,0xAE,0x4A,0x00
        defb 0x00,0x00,0xAA,0xAA,0x4A,0xA6,0xA2,0x0C,0x02,0x04,0xE4,0x28,0x44,0x84,0xE2,0x00
        defb 0x48,0x44,0x44,0x02,0x44,0x44,0x48,0x00,0x00,0x00,0xC0,0x60,0x00,0x00,0x00,0x00
