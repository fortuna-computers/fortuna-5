#include "init.h"

#include "ram.h"
#include "z80.h"
#include "bus.h"
#include "../bios/bios.avr.h"

void initialize()
{

    bus_reset_set(0);

    for (size_t i = 0; i < bios_bin_len; ++i)
        ram_set(i, pgm_read_byte(&(bios_bin[i])));

    z80_reset();

}