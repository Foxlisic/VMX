; ----------------------------------------------------------------------
; Вывод информации на дисплей
; ----------------------------------------------------------------------

; Очистить экран в цвет A :: 38B (11B + 27B)
cls:    ld      (CURRENTCLR), a
        ld      b, a
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
        ld      bc, $6003           ; 96 на атрибуты, 3*256 на экран
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
        ld      hl, $0000           ; Инициализировать курсор
        ld      (CURBLINK), hl      ; Курсора нет на экране
        call    locate
        ret

; ----------------------------------------------------------------------
; Печать значения числа A в HEX-формате (B становится 0)
; ----------------------------------------------------------------------

print8: push    af
        push    bc
        ld      b, 2
        push    af                  ; Сначала печатается старший ниббл
        rrca
        rrca
        rrca
        rrca
.a0:    and     0x0F                ; Убираются старшие 4 бита
        cp      10
        jr      c, .a1
        add     a, 7
.a1:    add     a, '0'
        call    prn
        dec     b                   ; Если B=1, то был напечатан младший ниббл
        jr      z, .a2
        pop     af
        jr      .a0
.a2:    pop     bc
        pop     af
        ret

; ----------------------------------------------------------------------
; Печать символа A в (Y,X) в телетайп-режиме с прокруткой
; ----------------------------------------------------------------------

prn:    push    bc
        push    de
        push    hl
        cp      10
        jr      z, .ent
        call    pchr
        ld      a, (LOCYX)
        inc     a
        ld      l, a                ; L=X++
        cp      64
.ent:   ld      a, (LOCYX+1)        ; H=Y
        ld      h, a
        jr      nz, .m1             ; X=64?
        ld      l, 0                ; L=0
        inc     a
        ld      h, a                ; Y++
        cp      24
        jr      nz, .m1             ; Y<24?
        dec     h
        push    hl
        ld      hl, (LOCYX)
        call    curhide
        call    scrollup
        pop     hl
.m1:    call    locate
        pop     hl
        pop     de
        pop     bc
        ret

; ----------------------------------------------------------------------
; Сдвиг страницы на 8 пикселей вверх
; ----------------------------------------------------------------------

scrollup:

        push    hl
        push    de
        push    bc
        push    af
        ld      hl, $4000       ; Перемотка экрана наверх
        ld      de, $4020
        ld      a, 3
        ld      c, 0
.n3:    push    af
.n2:    ld      b, 8
        push    de              ; Сохранить DE/HL
        push    hl
.n1:    ld      a, (de)         ; Скопировать букву
        ld      (hl), a
        inc     h
        inc     d
        djnz    .n1
        pop     hl              ; Восстановить HL/DE
        pop     de
        inc     hl
        inc     de
        ld      a, e
        and     a
        jr      nz, .x1
        ld      a, d
        add     7
        ld      d, a
.x1:    dec     c
        jr      nz, .n2
        ld      a, h
        add     7
        ld      h, a
        pop     af
        cp      2
        jr      nz, .x2
        ld      c, $e0
.x2:    dec     a
        jr      nz, .n3
        xor     a           ; Очистка нижней строки
        ld      h, $50      ; Банк
        ld      c, $08      ; 8 строк
.m2:    ld      l, $e0      ; 7-я строка
        ld      b, $20      ; 32 символа
.m1:    ld      (hl), a     ; Удалить область
        inc     hl
        djnz    .m1
        dec     c           ; Здесь будет H++
        jr      nz, .m2
        ld      bc, 768-32  ; Сдвиг атрибутов
        ld      e, $20
.m3:    ex      de, hl
        ldir
        ex      de, hl
        ld      b, $20      ; Заполнить нижнюю строку атрибутов
        ld      a, (CURRENTCLR)
.m4:    ld      (hl), a
        inc     hl
        djnz    .m4
        pop     af
        pop     bc
        pop     de
        pop     hl
        ret

; ----------------------------------------------------------------------
; Вычислить HL по координатам H=Y L=X
; ----------------------------------------------------------------------
; H ...bb000
; L aaaxxxxx
; Y    bbaaa

ploc:   ld      a, l
        and     a, $3E              ; L[5:1] -> L[4:0]
        rrca
        ld      l, a
        ld      a, h
        rrca
        rrca
        rrca
        and     a, $E0              ; H[2:0] -> L[7:5]
        or      a, l
        ld      l, a                ; L=aaaxxxxx
        ld      a, h
        and     a, $18
        add     a, $40
        ld      h, a                ; H[2:0]=0, H[6]=1
        ret

; ----------------------------------------------------------------------
; Печать символа A в LOCYX=(Y,X)
; ----------------------------------------------------------------------

pchr:   sub     a, $20
        ld      de, fontrom         ; Вычислить начальную позицию
        ld      c, 0                ; C=0
        ld      h, c                ; H=0
        ld      l, a
        srl     l
        add     hl, hl
        add     hl, hl
        add     hl, hl
        add     hl, de
        ex      de, hl              ; DE = 8*((A - 0x20) >> 1) + fontrom
        rrca
        rl      c                   ; C[2] Сдвиг символа вправо
        ld      a, (LOCYX)
        rrca
        rl      c                   ; C[1] Нечетный столбец
        ld      a, (CURBLINK)
        cp      25
        rl      c                   ; C[0]=1 курсора не видно
        ld      hl, (LOCYX)
        call    ploc
        ld      b, 8                ; Высота символа 8 строк
.m4:    ld      a, (de)             ; Прочесть источник данных
        bit     2, c                ; Тест бита конфигурации
        jr      nz, .m1             ; Если там 1, то тогда знакоместо не двигать
        rrca                        ; Если 0, то сдвинуть знакоместо на 4 бита вправо
        rrca
        rrca
        rrca
.m1:    bit     0, c                ; Рисовать или не рисовать курсор как подложку
        jr      nz, .m5
        xor     CURFORM
.m5:    and     $0F                 ; В любом случае срезать старшие 4 бита изображения
        push    bc
        bit     1, c
        jr      nz, .m2             ; Вывод символа слева или справа
        rlca                        ; Сдвинуть на 4 влево
        rlca
        rlca
        rlca
        ld      b, a
        ld      a, (hl)
        and     $0F                 ; Срезать 4 пикселя (были) слева
        jr      .m3
.m2:    ld      b, a
        ld      a, (hl)
        and     $F0                 ; Срезать 4 пикселя (были) справа
.m3:    or      b                   ; Наложить пиксели
        ld      (hl), a             ; Запись обратно
        pop     bc
        inc     de
        inc     h
        djnz    .m4
        ret

; ----------------------------------------------------------------------
; Установка курсора H=Y L=X
; ----------------------------------------------------------------------

locate: push    hl
        call    curhide
        ld      a, 50
        ld      (CURBLINK), a
        pop     hl
        ld      (LOCYX), hl
        call    curswitch.m0
        ret

; Скрыть курсор
curhide:ld      a, (CURBLINK)
        cp      25
        ret     c
        call    curswitch           ; Если показан, скрыть его
        ld      a, 24
        ld      (CURBLINK), a       ; Курсор сейчас скрыт
        ret

; Показать или скрыть курсор в позиции (LOCYX)
curswitch:

        ld      hl, (LOCYX)
.m0:    push    hl
        push    af
        ld      c, l
        call    ploc
        ld      b, 8
        bit     0, c
        ld      c, 16*CURFORM       ; Форма курсора на четной линии
        jr      z, .m1
        ld      c, CURFORM          ; Форма курсора на нечетной линии
.m1:    ld      a, (hl)
        xor     c
        ld      (hl), a
        inc     h
        djnz    .m1
        pop     af
        pop     hl
        ret

; ----------------------------------------------------------------------
; Мерцание курсора
; ----------------------------------------------------------------------

blink_irq:

        ld      a, (CURBLINK)       ; Текущий таймер мерцания
        cp      25                  ; Если он был 25, то станет 24
        jr      nz, .m1
        call    curswitch           ; И это значит, что курсор надо скрыть
.m1:    and     a                   ; А также если из 0 становится 49
        jr      nz, .m2
        ld      a, 50               ; То A=50 сначала
        call    curswitch           ; И курсор показать
.m2:    dec     a                   ; Уменьшить таймер на -1
        ld      (CURBLINK), a
        ld      hl, TIMER_LO
        inc     (hl)                ; Отсчитывать время
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
