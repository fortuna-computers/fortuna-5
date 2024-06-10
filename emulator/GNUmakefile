FDBG_SRC = ../fortuna-debugger/src
BIOS = ../bios

CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I${FDBG_SRC}/library/server -I${FDBG_SRC}/library/common -I${FDBG_SRC}/contrib/nanopb -I${BIOS}
LDFLAGS =

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

all: f5-emulator

${BIOS}/bios.h:
	$(MAKE) -C ${BIOS}

emulator.o: ${BIOS}/bios.h

${FDBG_SRC}/library/server/libfdbg-server.a:
	$(MAKE) -C ${FDBG_SRC}/library/server

f5-emulator: emulator.o z80/Z80.o ${FDBG_SRC}/library/server/libfdbg-server.a
	$(CC) -o $@ $^ ${LDFLAGS}

clean:
	rm -f emulator.o z80/Z80.o f5-emulator