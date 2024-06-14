    ; ei      ; fb
    ; im 1    ; ed 56
    nop     ; 00
again:
    in      a, (0x3)
    jr      again

    nop
    nop
    ; ld a, 'A'
    ; out (0x3), a
    nop