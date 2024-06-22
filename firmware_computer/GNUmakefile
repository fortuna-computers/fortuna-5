#
# configuration
#

PROJECT=fortuna5-dbg

VENDOR_ID  := 1a86
PRODUCT_ID := 7523

#
# directories
#

FDBG = ../fortuna-debugger
FDBG_COMMON = ${FDBG}/src/library/common
FDBG_SERVER = ${FDBG}/src/library/server
FDBG_NANOPB = ${FDBG}/src/contrib/nanopb
BIOS = ../bios

#
# flags
#

SERIAL_PORT=$(shell ${FDBG}/findserial.py ${VENDOR_ID} ${PRODUCT_ID})

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

AVRDUDE_FLAGS=-p ${MCU_AVRDUDE} -c ${AVRDUDE_CONFIG} -b 115200 -D -V -P ${SERIAL_PORT}

WARNINGS=-Wall -Wextra \
	 -Wformat=2 -Wno-unused-parameter -Wshadow \
	 -Wwrite-strings -Wshadow -Wstrict-aliasing \
	 -Wredundant-decls -Wmissing-include-dirs -Wlogical-op -Wno-array-bounds
CPPFLAGS=-I. -I${FDBG_COMMON} -I${FDBG_SERVER} -I${FDBG_NANOPB} -DMICROCONTROLLER=1 -Os -DF_CPU=${F_CPU} -DBAUD=${BAUD} -DBAUD_TOL=${BAUD_TOL} -mmcu=${MCU_GCC} -ffunction-sections -fdata-sections -mcall-prologues ${WARNINGS}
CXXFLAGS=-std=c++17


#
# programs
#

CC = avr-gcc
CXX = avr-g++

#
# sources
#

OBJECTS=main.avr.o uart.avr.o bus.avr.o ram.avr.o z80.avr.o io.avr.o run.avr.o debugger.avr.o init.avr.o debug.o \
		${FDBG_NANOPB}/pb_common.avr.o ${FDBG_NANOPB}/pb_encode.avr.o ${FDBG_NANOPB}/pb_decode.avr.o \
       	${FDBG_SERVER}/to-computer.nanopb.avr.o ${FDBG_SERVER}/to-debugger.nanopb.avr.o \
       	${FDBG_SERVER}/libfdbg-server.avr.o ${FDBG_SERVER}/srvcomm.avr.o

#
# Rules
#

all: ${PROJECT}.hex

# compile object files as *.avr.o

%.avr.o: %.c
	$(CC) -c -o $@ $< ${CFLAGS} ${CPPFLAGS}

# generate BIOS header file

${BIOS}/bios.avr.h:
	$(MAKE) -C ${BIOS}

init.avr.o: ${BIOS}/bios.avr.h

# build the protobufs

server_build:
	$(MAKE) -C ${FDBG_SERVER} nanopb

${OBJECTS}: | server_build

# generate the microcontroller image

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
	rm -f *.o ${OBJECTS} ${PROJECT}.elf ${PROJECT}.hex

# vim: set ts=8 sts=8 sw=8 noexpandtab:
