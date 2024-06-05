    include nop.asm

    nop
    call sr

stp:
    jr  stp


sr:
    nop
    ret

    include nmi.z80