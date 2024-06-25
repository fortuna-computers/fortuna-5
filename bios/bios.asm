hh:
    ld  a, '1'
    out (0x3), a

    ld  a, '2'
    out (0x3), a

    ld  a, '3'
    out (0x3), a

    ld  a, '4'
    out (0x3), a

    ld  a, '5'
    out (0x3), a

start:
    in  a, (0x3)

    or  a, a
    jr  z, start

    out (0x3), a

    jr  start


    include nmi.z80