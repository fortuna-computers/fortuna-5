VASM := ../vasm/vasmz80_oldstyle

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	VASM := ${VASM}_macos
endif

all: bios.bin bios.h bios.avr.h

bios.bin: $(wildcard *.z80)
	${VASM} -chklabels -Llo -ignore-mult-inc -nosym -x -Fbin -o $@ bios.asm

bios.h: bios.bin
	xxd -i $^ > $@

bios.avr.h: bios.h
	{ echo '#include <avr/pgmspace.h>'; cat $^; } >$@
	sed -i '' 's/unsigned char bios_bin\[\]/const unsigned char bios_bin\[\] PROGMEM/g' $@

clean:
	rm -f bios.bin bios.h bios.avr.h