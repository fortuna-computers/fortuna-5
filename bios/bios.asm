    include nop.asm

    inc a
    inc a
    inc a
    inc a
    call sr

stp:
    jr  stp


sr:
    nop
    ret

    include nmi.z80