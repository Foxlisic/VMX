; RST#00
        di              ; 1
        xor     a       ; 1
        ld      sp, 0   ; 3
        jp      reset   ; 3
; RST#08-$30
        defb    0,0,0,0,0,0,0,0 ; #08
        defb    0,0,0,0,0,0,0,0 ; #10
        defb    0,0,0,0,0,0,0,0 ; #18
        defb    0,0,0,0,0,0,0,0 ; #20
        defb    0,0,0,0,0,0,0,0 ; #28
        defb    0,0,0,0,0,0,0,0 ; #30
rst38:  halt
        ei
        reti

reset:  ei
        ld      a, 7
        out     ($fe), a
        jr      $
