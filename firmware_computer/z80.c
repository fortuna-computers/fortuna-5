#include "z80.h"

#include <stdint.h>
#include <avr/cpufunc.h>

#include "bus.h"

void z80_init()
{
    bus_reset_set(0);
}

uint16_t z80_reset()
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

    return z80_single_step();
}

void z80_cycle()
{
    bus_clk_set(1);
    _NOP();
    bus_clk_set(0);
    _NOP();
}

void z80_release_bus()
{
    if (bus_reset_get() == 0 || bus_busak_get() == 0)
        return;  // already without bus control

    bus_busrq_set(0);
    while (bus_busak_get() != 0)
        z80_cycle();
}

uint16_t z80_single_step()
{
    bus_busrq_set(1);   // make sure we're not requesting the bus
    z80_cycle();

    while (bus_m1_get() != 0)
        z80_cycle();

    uint16_t pc = bus_addr_get();

    z80_cycle();

    // TODO - check previous instruction

    return pc;
}