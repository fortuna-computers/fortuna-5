#!/bin/bash

VASM=../vasm/vasmz80_oldstyle
if [[ "$OSTYPE" == "darwin"* ]]; then
  VASM=${VASM}_macos
fi

rm -f bios.bin

${VASM} -chklabels -Llo -ignore-mult-inc -nosym -x -Fbin -o bios.bin bios.asm
xxd -i bios.bin > bios.h