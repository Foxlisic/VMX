        org     0
; ----------------------------------------------------------------------
KEYBUF      EQU     0x5B00  ; Адрес буфера состояния клавиш
KEYBFIFO    EQU     0x5B08  ; Клавиатурный буфер FIFO
TMP16       EQU     0x5B10  ; Временное 16-битное значение
LOCYX       EQU     0x5B12  ; Текущее положение курсора
CURBLINK    EQU     0x5B14  ; 0..24 Курсора нет 25..49 Курсор проявлен
TIMER_LO    EQU     0x5B15  ; Счетчик кадров
SD_TYPE     EQU     0x5B16  ; 0=Карта не обнаружена 1=SD1 2=SD2 3=SDHC
SD_LBA      EQU     0x5B17  ; 4 байта запрос LBA
SD_ARG32    EQU     0x5B1B  ; 4 байта (внутренний аргумент)
SD_TMP1     EQU     0x5B1F  ; Временное 8 битное значение
CURRENTCLR  EQU     0x5B20  ; Текущий цвет
; ---------------------------------------------------------------------
SD_SECTOR   EQU     0x5C00  ; 512 байт
; ---------------------------------------------------------------------
CURFORM     EQU     0x0F    ; Форма курсора
SD_DAT      EQU     0x3F    ; Порт с данными SD
SD_CMD      EQU     0x7F    ; Порт статуса SD
SD_CE0      EQU     2
SD_CE1      EQU     3
SD_TIMEOUT  EQU     4095
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
        call    blink_irq
        pop     bc
        pop     de
        pop     hl
        pop     af
        ei
        reti

; ---------------------------------------------------------------------
reset:  im      0
        ld      hl, $5B00               ; Заполнить нулями BDA
        xor     a
.clr:   ld      (hl), a
        inc     l
        jr      nz, .clr
        ld      a, (0*8) + 4            ; Очистка экрана
        call    cls
        call    kbd_init

        ; Загрузка бутсектора

        ei

.x:     call    kbd_fetch
        jr      z, .x
        call    prn
        jr      .x

        include "keyboard.asm"
        include "display.asm"
        include "sdcard.asm"
