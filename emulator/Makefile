CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra $(shell pkg-config --cflags libfdbg-server)
LDFLAGS = $(shell pkg-config --libs libfdbg-server)

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

all: f5-emulator

f5-emulator: emulator.o
	$(CC) -o $@ $^ ${LDFLAGS}

clean:
	rm -f emulator.o s-emulator
