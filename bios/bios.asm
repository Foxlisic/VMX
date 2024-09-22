; RST#00
        di              ; 1
        xor     a       ; 1
        ld      sp, 0   ; 3
        jp      reset   ; 3
;
reset:  halt
