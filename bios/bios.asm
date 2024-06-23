hh:
    ld  a, 'H'
    out (0x3), a
    ld  a, 'e'
    out (0x3), a

    ld  a, 'l'
    out (0x3), a
    jr  hh

    ld  a, 'l'
    out (0x3), a
    ld  a, 'o'
    out (0x3), a

start:
    ;in  a, (0x3)
    ;or  a, a
    ;jr  z, start

    ;out (0x3), a
    jr  start


    ;include nmi.z80