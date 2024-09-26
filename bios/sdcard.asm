; ----------------------------------------------------------------------
; Чтение сектора с диска в память (адрес в памяти HL)
; ----------------------------------------------------------------------
sdread:

        ld      (TMP16), hl
        call    sdinit
        ret     c                   ; Возникла ошибка при инициализации
        ld      a, (SD_TYPE)        ; Проверить тип SD карты
        cp      3
        jr      z, .s1              ; Тип карточки 3, валидно
        scf
        ret
.s1:    ld      hl, (SD_LBA)
        ld      (SD_ARG32), hl
        ld      hl, (SD_LBA+2)
        ld      (SD_ARG32+2), hl
        ld      a, 17               ; CMD17: READ sector
        call    sdcomm
        ret     c
        ld      hl, 4095            ; Ожидание ответа FEh
.r1:    call    sdget               ; Читать следуюший байт
        cp      $FE
        jr      z, .ok              ; Если там FEh то все верно
        cp      $FF
        jr      nz, .err            ; Если там не FFh, то это ошибка
        dec     hl
        ld      a, h
        or      l
        jr      nz, .r1             ; Повтор 4095 раз
.err:   scf                         ; Ошибка
        ret
.ok:    ld      de, 512             ; Читать 512 байт в память
        ld      hl, (TMP16)
.r2:    call    sdget
        ld      (hl), a
        inc     hl
        dec     de
        ld      a, d
        or      e
        jr      nz, .r2
        ld      a, 3                ; И отключить чип
        call    sdcmd               ; CE1
        scf
        ccf
        ret

; ----------------------------------------------------------------------
; Инициализация SD карты
; ----------------------------------------------------------------------

sdinit: in      a, (SD_CMD)
        rlca
        ret     nc                  ; Нет таймаута == инициализировано
        xor     a
        ld      (SD_TYPE), a
        call    sdcmd               ; 80T в медленном режиме, включение SPI
        ld      hl, $0000
        ld      (SD_ARG32+0), hl
        ld      (SD_ARG32+2), hl
        ld      a, 0                ; Отослать CMD0(00000000h)
        call    sdcomm
        ret     c
        cp      1
        jr      z, .a1              ; Должен быть R1_IDLE код в ответе
        scf
        ret

        ; Запросить CMD8 с аргументом $01AA
.a1:    ld      hl, $01AA
        ld      (SD_ARG32), hl
        ld      a, 8                ; CMD8
        call    sdcomm
        ret     c
        bit     2, a                ; & R1_ILLEGAL_COMMAND ?
        jr      z, .a2
        ld      a, 1                ; Если да, то это SD1
        ld      (SD_TYPE), a        ; SD1
        jr      .acmd
.a2:    ld      b, 4                ; Прочесть 4 байта ответа
.a3:    call    sdget
        djnz    .a3
        cp      $AA
        ld      a, 2
        ld      (SD_TYPE), a
        jr      z, .acmd            ; На 4-м байте должен вернуть ответ AAh
        scf
        ret

        ; Отослать ACMD
.acmd:  ld      hl, 4095
.a7:    push    hl
        ld      hl, $0000
        ld      (SD_ARG32), hl
        ld      (SD_ARG32+2), hl
        ld      a, 55
        call    sdcomm
        ld      a, (SD_TYPE)        ; IF SDType=2 THEN $40 ELSE $00
        cp      2
        ld      a, 0x00
        jr      nz, .a5
        ld      a, 0x40
.a5:    ld      (SD_ARG32+3), a
        ld      a, 0x29
        call    sdcomm
        pop     hl
        ret     c                   ; Возникла ошибка
        and     a                   ; = R1_READY_STATE?
        jr      z, .a6
        dec     hl
        ld      a, h
        or      l
        jr      nz, .a7
        scf
        ret                         ; От ACMD ответа не было

        ; Если SD2, то запросить SDHC
.a6:    ld      a, (SD_TYPE)
        cp      2
        jr      nz, .end
        ld      hl, $0000
        ld      (SD_ARG32), hl
        ld      (SD_ARG32+2), hl
        ld      a, 58               ; CMD58(0)
        call    sdcomm
        ret     c

        ; Определить что это SDHC по наличию 2х бит в 7 и 6 битах
        call    sdget
        and     $c0
        cp      $c0
        jr      nz, .a8
        ld      a, 3
        ld      (SD_TYPE), a        ; SDHC карта
.a8:    call    sdget               ; Дочитать 3 байта
        call    sdget
        call    sdget
.end:   ld      a, 3
        call    sdcmd               ; CE=1
        scf
        ccf
        ret

; ----------------------------------------------------------------------
; Отослать команду A с 32х битным параметром
; ----------------------------------------------------------------------

sdcomm: ld      (SD_TMP1), a    ; Сохранить команду
        ld      a, SD_CE0
        call    sdcmd           ; Активация чипа

        ; FOR i=0 TO SD_TIMEOUT-1: IF (GET() == 0xFF) BREAK; NEXT i
        ld      hl, SD_TIMEOUT
.k0:    call    sdget
        cp      $FF
        jr      z, .k1          ; Должен быть ответ FFh
        dec     hl
        ld      a, h
        or      l
        jr      nz, .k0         ; Считать таймаут
        scf
        ret

        ; Отослать команду
.k1:    ld      a, (SD_TMP1)
        or      a, $40
        call    sdput

        ; Записать 32х битное значение
        ld      b, 4
        ld      hl, SD_ARG32+3  ; Сначала старшие
.k2:    ld      a, (hl)
        call    sdput
        dec     hl              ; Потом младшие байты
        djnz    .k2

        ; В зависимости от команды, выслать CRC
        ld      a, (SD_TMP1)
        and     a
        ld      b, 0x95
        jr      z, .k3          ; CMD0, CRC=95h
        cp      8
        ld      b, 0x87
        jr      z, .k3          ; CMD8, CRC=87h
        ld      b, 0xFF         ; Все остальные CRC=FFh
.k3:    ld      a, b
        call    sdput

        ; Ожидать снятие флага BSY у чипа (256 проверок)
        ld      h, 0
.k4:    call    sdget
        jp      p, .k5
        dec     h
        jr      nz, .k4
        scf
        ret

        ; CF=0, функция в порядке, ответ -> A
.k5:    scf
        ccf
        ret

; ----------------------------------------------------------------------
; Отсылка команды/данных к SD
; ----------------------------------------------------------------------

        ; Ожидать BSY=0
sdwait: push    af
.cc:    in      a, (SD_CMD)
        rrca
        jr      c, .cc
        pop     af
        ret

        ; Выслать команду
sdcmd:  call    sdwait
        out     (SD_CMD), a
        ret

        ; Выслать A и принять данные -> A
sdget:  ld      a, 0xFF
sdput:  call    sdwait
        out     (SD_DAT), a
        call    sdwait
        in      a, (SD_DAT)
        and     a
        ret
