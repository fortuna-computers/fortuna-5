#include "init.h"

#include "ram.h"
#include "z80.h"
#include "bus.h"
#include "../bios/bios.h"

void initialize()
{
    bus_init_led_set(1);

    bus_reset_set(0);

    for (size_t i = 0; i < bios_bin_len; ++i)
        ram_set(i, bios_bin[i]);

    z80_reset();

    bus_init_led_set(0);
}