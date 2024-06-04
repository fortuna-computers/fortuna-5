#include "z80.h"

#include <stdint.h>
#include <avr/cpufunc.h>

#include "bus.h"

void z80_init()
{
    bus_reset_set(0);
}

void z80_reset()
{
    bus_reset_set(0);
    for (uint8_t i = 0; i < 50; ++i)
        z80_cycle();
    bus_reset_set(1);

    bus_nmi_set(1);
    bus_int_set(1);
    bus_clk_set(0);
    bus_busrq_set(1);
    bus_wait_set(1);  // pull-up
}

void z80_cycle()
{
    bus_clk_set(1);
    _NOP();
    bus_clk_set(0);
    _NOP();
}