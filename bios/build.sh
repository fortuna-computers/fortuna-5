#!/bin/bash

VASM=../vasm/vasmz80_oldstyle
if [[ "$OSTYPE" == "darwin"* ]]; then
  VASM=${VASM}_macos
fi

rm -f bios.bin bios.h bios.avr.h

# compile bios
${VASM} -chklabels -Llo -ignore-mult-inc -nosym -x -Fbin -o bios.bin bios.asm

# generate headers
xxd -i bios.bin > bios.h

# generate AVR header
{ echo '#include <avr/pgmspace.h>'; cat bios.h; } >bios.avr.h
sed -i='' 's/unsigned char bios_bin\[\]/const unsigned char bios_bin\[\] PROGMEM/g'  bios.avr.h