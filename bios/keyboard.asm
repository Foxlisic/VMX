; ----------------------------------------------------------------------
; Обработчики клавиатуры
; ----------------------------------------------------------------------

; Очистка клавиатурного буфера
kbd_init:

        ld      hl, KEYBUF
        ld      a, 0xFF
        call    .c2
        inc     a
.c2:    ld      b, 8
.c1:    ld      (hl), a
        inc     hl
        djnz    .c1
        ret

; Цикл проверки изменений клавиатуры
kbd_irq:

        ld      bc, $7FFE
        ld      hl, kbdkey      ; Базовая таблица символов (40 символов)
        ld      de, 40          ; 40 символов размер таблицы
        in      a, (c)          ; Проверить клавишу SS
        bit     1, a
        jr      nz, .ss
        add     hl, de          ; Если она нажата, то +80
        add     hl, de
.ss:    ld      b, $FE
        in      a, (c)          ; Проверка клавиши CS
        bit     0, a
        jr      nz, .cs
        add     hl, de          ; +40 если нажато
.cs:    ex      de, hl
        ld      hl, KEYBUF      ; 8 байт статусов кнопок
        ld      b, $FE          ; Бит 0 очищен в 0 на старте
.r1:    ld      c, $FE
        in      a, (c)
        ld      c, a            ; Новое состояние порта
        xor     (hl)            ; Сверить со старым
        ld      (hl), c
        inc     hl
        push    bc              ; Сохранить B
        ld      b, 5
.r2:    push    bc              ; Сохранить C
        rrca
        jr      nc, .s1         ; CF=1 Клавиша изменилась
        rrc     c
        jr      c, .s1          ; CF=0 Клавиша нажата; CF=1 Отпущена
        push    af
        ld      a, (de)
        and     a, a            ; CAPS=0 или SHIFT=0 пропускать
        call    nz, kbd_add     ; Добавить клавишу в FIFO
        pop     af
.s1:    inc     de
        pop     bc
        rrc     c               ; Сдвинуть C
        djnz    .r2             ; 5 колонок
        pop     bc
        rlc     b               ; 8 строк
        jr      c, .r1
        ret

; ----------------------------------------------------------------------
; Добавить клавишу A в FIFO буфер
; ----------------------------------------------------------------------

kbd_add:push    de
        push    hl
        ld      hl, KEYBFIFO    ; Ссылка на FIFO
        ld      d, 0
        ld      e, (hl)         ; Существующее количество данных
        push    af              ; Проверка на превышение размера буфера
        ld      a, e
        cp      $6
        jr      nz, .s1
        dec     (hl)            ; -1 Откатить
.s1:    inc     (hl)            ; +1
        pop     af
        add     hl, de          ; Переместить указатель для сохранения
        inc     hl
        ld      (hl), a         ; Сохранить новый код клавиши
        pop     hl
        pop     de
        ret

; ----------------------------------------------------------------------
; Прочитать принятую клавишу -> A
; ----------------------------------------------------------------------

kbd_fetch:

        ld      a, (KEYBFIFO)
        and     a
        ret     z
        di
        dec     a
        ld      (KEYBFIFO), a
        ld      de, KEYBFIFO+1
        ld      a, (de)
        ld      hl, KEYBFIFO+2
        ld      bc, 6
        ldir
        ei
        and     a
        ret

; ----------------------------------------------------------------------
; Данные для клавиатуры
; ----------------------------------------------------------------------
kbdkey:

        ; CS=0 SS=0
        defb    0,"zxcv"    ; A8  CAP
        defb    "asdfg"     ; A9
        defb    "qwert"     ; A10
        defb    "12345"     ; A11
        defb    "09876"     ; A12
        defb    "poiuy"     ; A13
        defb    10,"lkjh"   ; A14
        defb    " ",0,"mnb" ; A15 SYM

        ; CS=1 SS=0
        defb    0,"ZXCV"    ; A8  CAP
        defb    "ASDFG"     ; A9
        defb    "QWERT"     ; A10
        defb    "!@#$%"     ; A11
        defb    ")(*&^"     ; A12
        defb    "POIUY"     ; A13
        defb    10,"LKJH"   ; A14
        defb    " ",0,"MNB" ; A15 SYM

        ; CS=0 SS=1
        defb    0,  ':',127,'?','/'
        defb    '~','|','\\','{','}'
        defb    0,  0,  0,  '<','>'
        defb    0,  0,  0,  0,  0
        defb    0,  0,  0,  0,  0
        defb    0,  ';',0,  0,  0
        defb    0,  '=',0,  '-',0
        defb    0,  0,  '.',',',' '

        ; CS=1 SS=1
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0
        defb    0, 0, 0, 0, 0

