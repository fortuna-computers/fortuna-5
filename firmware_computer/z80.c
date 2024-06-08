#include "z80.h"

#include <stdint.h>
#include <avr/cpufunc.h>
#include <stddef.h>

#include "bus.h"
#include "ram.h"
#include "io.h"

volatile Z80_Status z80;

void z80_init()
{
    bus_reset_set(0);
    z80.pc = 0;
    z80.next_op = 0;
    z80.updated = false;
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
    bus_cwait_set(1);  // pull-up
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

static void z80_manage_iorq()
{
    uint8_t port;
    uint8_t data;

    MemPins mp = bus_mem_get();
    if (mp.wr == 0) {
        port = bus_addr_get() & 0xff;
        data = bus_data_get();
        io_out(port, data, true);
    } else if (mp.rd == 0) {
        port = bus_addr_get() & 0xff;
        data = io_in(port, true);
        bus_data_control(WRITE);
        bus_data_set(data);
    }

    bus_cwait_set(0);
    while (bus_iorq_get() == 0)
        z80_cycle();
    bus_data_control(READ);
    bus_cwait_set(1);
}

void z80_single_step()
{
    bus_busrq_set(1);   // make sure we're not requesting the bus
    z80_cycle();

    while (bus_m1_get() != 0) {
        z80_cycle();

        if (bus_iorq_get() == 0)
            z80_manage_iorq();
    }

    z80.pc = bus_addr_get();
    z80.next_op = bus_data_get();

    z80_cycle();

    // when calling a Z80 combined instruction, a second step must be made
    static uint8_t previous_instruction = 0x00;
    bool combined_instruction = (previous_instruction == 0xcb || previous_instruction == 0xdd || previous_instruction == 0xed || previous_instruction == 0xfd);
    previous_instruction = bus_data_get();
    if (combined_instruction)
        z80_single_step();

    z80.updated = false;
}

void z80_full_step()
{
    // call NMI subroutine
    bus_nmi_set(0);
    z80_single_step();
    z80_single_step();
    bus_nmi_set(1);

    // run NMI subroutine until it reaches 'ret'
    while (z80.next_op != 0xc9)
        z80_single_step();

    // TODO - get registers
    z80.af  = ram_get(0x100) | ((uint16_t) ram_get(0x101) << 8);
    z80.bc  = ram_get(0x102) | ((uint16_t) ram_get(0x103) << 8);
    z80.de  = ram_get(0x104) | ((uint16_t) ram_get(0x105) << 8);
    z80.hl  = ram_get(0x106) | ((uint16_t) ram_get(0x107) << 8);
    z80.afx = ram_get(0x108) | ((uint16_t) ram_get(0x109) << 8);
    z80.bcx = ram_get(0x10a) | ((uint16_t) ram_get(0x10b) << 8);
    z80.dex = ram_get(0x10c) | ((uint16_t) ram_get(0x10d) << 8);
    z80.hlx = ram_get(0x10e) | ((uint16_t) ram_get(0x10f) << 8);
    z80.ix  = ram_get(0x110) | ((uint16_t) ram_get(0x111) << 8);
    z80.iy  = ram_get(0x112) | ((uint16_t) ram_get(0x113) << 8);
    z80.sp  = ram_get(0x114) | ((uint16_t) ram_get(0x115) << 8);
    z80.i = 0; // TODO

    for (size_t i = 0; i < STACK_SZ; ++i)
        z80.stack[i] = ram_get(z80.sp + i);

    // return from NMI
    z80_single_step();

    z80.updated = true;
}

uint8_t z80_next_instruction_size()
{
    static const uint8_t CALL_OPS[] = { 0xC4, 0xCC, 0xCD, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC };  // TODO - move this to PROGMEM
    static const uint8_t RST_OPS[] = { 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF };

    for (size_t i = 0; i < sizeof(CALL_OPS); ++i) {
        if (z80.next_op == CALL_OPS[i]) {
            return 3;
        }
    }
    for (size_t i = 0; i < sizeof(RST_OPS); ++i) {
        if (z80.next_op == RST_OPS[i]) {
            return 1;
        }
    }

    return 0;
}