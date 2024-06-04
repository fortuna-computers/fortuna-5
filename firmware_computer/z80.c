#include "z80.h"

#include <stdint.h>
#include <avr/cpufunc.h>
#include <stddef.h>

#include "bus.h"

static uint8_t last_op = 0;

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
    last_op = bus_data_get();

    z80_cycle();

    // when calling a Z80 combined instruction, a second step must be made
    static uint8_t previous_instruction = 0x00;
    bool combined_instruction = (previous_instruction == 0xcb || previous_instruction == 0xdd || previous_instruction == 0xed || previous_instruction == 0xfd);
    previous_instruction = bus_data_get();
    if (combined_instruction)
        pc = z80_single_step();

    return pc;
}

uint16_t z80_full_step()
{
    return 0;
}

uint8_t z80_next_instruction_size()
{
    static const uint8_t CALL_OPS[] = { 0xC4, 0xCC, 0xCD, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC };  // TODO - move this to PROGMEM
    static const uint8_t RST_OPS[] = { 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF };

    for (size_t i = 0; i < sizeof(CALL_OPS); ++i) {
        if (last_op == CALL_OPS[i]) {
            return 3;
        }
    }
    for (size_t i = 0; i < sizeof(RST_OPS); ++i) {
        if (last_op == RST_OPS[i]) {
            return 1;
        }
    }

    return 0;
}