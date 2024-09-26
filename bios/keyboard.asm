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

        ld      de, kbdkey      ; Коды клавиш CS=0 SS=0
        ld      bc, $FEFE
        in      a, (c)
        and     a, 1
        jr      nz, .s3         ; Если CS нажат, то подменить таблицу
        ld      de, kbdkey_cs
.s3:    ld      b, $7F
        in      a, (c)
        and     a, 2
        jr      nz, .s4         ; Нажат SS, он приоритетнее CS
        ld      de, kbdkey_ss
.s4:    ld      hl, KEYBUF      ; 8 байт статусов кнопок
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
        and     a, a
        jr      z, .s2          ; CAPS или SHIFT
        push    de
        push    hl
        ld      hl, KEYBFIFO    ; Ссылка на FIFO
        ld      d, 0
        ld      e, (hl)         ; Существующее количество данных
        push    af              ; Проверка на превышение размера буфера
        ld      a, e
        cp      $6
        jr      nz, .s5
        dec     (hl)            ; -1 Откатить
.s5:    inc     (hl)            ; +1
        pop     af
        add     hl, de          ; Переместить указатель для сохранения
        inc     hl
        ld      (hl), a         ; Сохранить новый код клавиши
        pop     hl
        pop     de
.s2:    pop     af
.s1:    inc     de
        pop     bc
        rrc     c               ; Сдвинуть C
        djnz    .r2             ; 5 колонок
        pop     bc
        rlc     b               ; 8 строк
        jr      c, .r1
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
kbdkey: ; CS=0 SS=0

        defb    0,"zxcv"    ; A8  CAP
        defb    "asdfg"     ; A9
        defb    "qwert"     ; A10
        defb    "12345"     ; A11
        defb    "09876"     ; A12
        defb    "poiuy"     ; A13
        defb    10,"lkjh"   ; A14
        defb    " ",0,"mnb" ; A15 SYM

kbdkey_cs: ; CS=1 SS=0

        defb    0,"ZXCV"    ; A8  CAP
        defb    "ASDFG"     ; A9
        defb    "QWERT"     ; A10
        defb    "!@#$%"     ; A11
        defb    ")(*&^"     ; A12
        defb    "POIUY"     ; A13
        defb    10,"LKJH"   ; A14
        defb    " ",0,"MNB" ; A15 SYM

kbdkey_ss: ; CS=? SS=0

        defb    1, 0,0,0,0
        defb    0, 0,0,0,0
        defb    0, 0,0,0,0
        defb    0, 0,0,0,0
        defb    0, 0,0,0,0
        defb    0, 0,0,0,0
        defb    0,'=',0,'-',0
        defb    0, 0,".,",0
