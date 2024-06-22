    ld  a, '*'
    out (0x3), a

start:
    in  a, (0x3)
    or  a, a
    jr  z, start

    out (0x3), a
    jr  start


    include nmi.z80