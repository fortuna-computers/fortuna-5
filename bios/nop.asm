    nop
    nop
    ld a, 'H'
    out (0x3), a
    in a, (0x3)
    nop