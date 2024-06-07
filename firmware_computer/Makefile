#
# Configuration
#

UC_SRC=/usr/local/src/fdbg/microcontroller
SERIAL_PORT=$(shell findserial.py 1a86 7523)

# Project name
PROJECT=fortuna5-dbg

# Object files (one for each source file)
OBJECTS=main.o uart.o bus.o ram.o z80.o io.o run.o debugger.o \
       	fdbg/pb_common.o fdbg/pb_encode.o fdbg/pb_decode.o fdbg/protobuf/to-computer.nanopb.o \
        fdbg/protobuf/to-debugger.nanopb.o fdbg/libfdbg-server.o

# Avrdude configuration to connect on the PI. See avrdude.conf.
AVRDUDE_CONFIG=wiring

# Microcontroller
MCU_GCC=atmega2560
MCU_AVRDUDE=atmega2560

# Microcontroller speed, in Hz. Used by avr-libc for delays.
F_CPU=16000000UL

# Serial speed, in baud. Used by avr-libc for UART speed calculation.
BAUD=115200
BAUD_TOL=3       # error tolerance

#
# Rules
#

AVRDUDE_FLAGS=-p ${MCU_AVRDUDE} -c ${AVRDUDE_CONFIG} -b 115200 -D -V -P ${SERIAL_PORT}
CC=avr-gcc
CXX=avr-g++
WARNINGS=-Wall -Wextra \
	 -Wformat=2 -Wno-unused-parameter -Wshadow \
	 -Wwrite-strings -Wshadow -Wstrict-aliasing \
	 -Wredundant-decls -Wmissing-include-dirs -Wlogical-op -Wno-array-bounds
CPPFLAGS=-I. -Ifdbg -Ifdbg/protobuf -DMICROCONTROLLER=1 -Os -DF_CPU=${F_CPU} -DBAUD=${BAUD} -DBAUD_TOL=${BAUD_TOL} -mmcu=${MCU_GCC} -ffunction-sections -fdata-sections -mcall-prologues ${WARNINGS}
CXXFLAGS=-std=c++17

all: FORCE ${PROJECT}.hex

FORCE:
	@mkdir -p fdbg
ifdef FDEBUGGER_PATH
	@mkdir -p fdbg/protobuf
	cp -R ${FDEBUGGER_PATH}/src/library/server/*.[ch] \
		  ${FDEBUGGER_PATH}/src/library/common/common.h \
		  ${FDEBUGGER_PATH}/src/contrib/nanopb/*.[ch] \
		  fdbg
	cp -R  ${FDEBUGGER_PATH}/src/library/protobuf/*.nanopb.[ch] fdbg/protobuf
else
	cp -R ${UC_SRC}/* fdbg
endif

${PROJECT}.hex: ${PROJECT}.elf
	avr-objcopy -j .text -j .data -O ihex -R .eeprom $< $@
	avr-size ${PROJECT}.hex

${PROJECT}.elf: ${OBJECTS}
	$(CXX) -mmcu=${MCU_GCC} -flto -o $@ $^ -Wl,--gc-sections
	avr-size -C --mcu=${MCU_GCC} ${PROJECT}.elf	

test-connection:
	avrdude ${AVRDUDE_FLAGS}

upload: ${PROJECT}.hex
	avrdude ${AVRDUDE_FLAGS} -U flash:w:$<:i

size: ${PROJECT}.elf
	avr-size -C --mcu=${MCU_GCC} ${PROJECT}.elf	

talk:
	@echo "Press Ctrl+A and then Shift+K to exit."
	@sleep 1
	screen ${SERIAL_PORT} ${BAUD}

clean:
	rm -f *.o ${PROJECT}.elf ${PROJECT}.hex
	rm -rf fdbg

# vim: set ts=8 sts=8 sw=8 noexpandtab:
