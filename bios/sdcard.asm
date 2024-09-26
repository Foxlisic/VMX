; ----------------------------------------------------------------------
; Чтение сектора с диска в память (адрес в памяти HL)
; ----------------------------------------------------------------------
sdread:

        ; Проверить Timeout=1
        in      a, (SD_CMD)
        rlca
        jr      nc, .s1
        call    sdinit
.s1:    ret

sdinit:

        xor     a
        call    sdcmd           ; 80T
        ld      hl, $0000
        ld      (SD_ARG32), hl
        ld      (SD_ARG32+2), hl
        ld      a, 0            ; CMD0
        call    sdcomm
        ret

; ----------------------------------------------------------------------
; Отослать команду A с 32х битным параметром
; ----------------------------------------------------------------------

sdcomm: ld      (TMP16), a      ; Сохранить команду
        ld      a, SD_CE0
        call    sdcmd           ; Активация чипа

        ; FOR i=0 TO SD_TIMEOUT-1: IF (GET() == 0xFF) BREAK; NEXT i
        ld      hl, SD_TIMEOUT
.k0:    call    sdget
        cp      $FF
        jr      z, .k1
        dec     hl
        ld      a, h
        or      l
        jr      nz, .k0
        ld      a, SD_ERR1
        scf
        ret

        ; Отослать команду
.k1:    ld      a, (TMP16)
        or      a, $40
        call    sdput

        ; Выслать 32х битное значение
        ld      b, 4
        ld      hl, SD_ARG32+3
.k2:    ld      a, (hl)
        call    sdput
        dec     hl
        djnz    .k2

        ; В зависимости от команды, выслать CRC
        ld      a, (TMP16)
        cp      0
        ld      b, 0x95
        jr      z, .k3
        cp      8
        ld      b, 0x87
        jr      z, .k3
        ld      b, 0xFF
.k3:    ld      a, b
        call    sdput

        ; Ожидать снятие флага BSY у чипа (256 проверок)
        ld      h, 0
.k4:    call    sdget
        jp      p, .k5
        dec     h
        jr      nz, .k4
        ld      a, SD_ERR2
        scf
        ret

        ; CF=0, функция в порядке, ответ -> A
.k5:    halt
        scf
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
