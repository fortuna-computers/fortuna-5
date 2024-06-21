#include "z80.h"

#include <stdint.h>
#include <avr/cpufunc.h>
#include <stddef.h>

#include "bus.h"
#include "ram.h"
#include "io.h"
#include "debug.h"

volatile Z80_Status z80;

void z80_init()
{
    bus_reset_set(0);
    z80.pc = 0;
    z80.last_op = 0;
    z80.next_op = 0;
    z80.updated = false;
    z80.has_next_interrupt = 0;
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

    DEBUG("Z80 reset.");
}

void z80_cycle()
{
    bus_clk_set(1);
    _NOP();
    bus_clk_set(0);
    _NOP();

    debug_putc('^');
}

void z80_release_bus()
{
    if (bus_reset_get() == 0 || bus_busak_get() == 0)
        return;  // already without bus control

    bus_busrq_set(0);
    while (bus_busak_get() != 0)
        z80_cycle();

    DEBUG("Bus released.");
}

static void z80_manage_iorq()
{
    DEBUG("  handling I/O");

    uint8_t port;
    uint8_t data;

    MemPins mp = bus_mem_get();
    if (mp.wr == 0 && mp.rd == 1) {   // output
        port = bus_addr_get() & 0xff;
        data = bus_data_get();
        DEBUG("    I/O output: [%02X] = %02X", port, data);
        io_out(port, data, true);
    } else if (mp.rd == 0 && mp.wr == 1) {  // input
        port = bus_addr_get() & 0xff;
        data = io_in(port, true);
        DEBUG("    I/O input: %02X <- [%02X]", data, port);
        bus_data_control(WRITE);
        bus_data_set(data);
    } else if (mp.wr == 1 && mp.rd == 1) {  // interrupt
        data = z80.next_interrupt;
        DEBUG("    I/O interrupt: %02X", data);
        bus_data_control(WRITE);
        bus_data_set(data);
    }

    DEBUG("    waiting for end of I/O (IORQ goes high)");
    bus_cwait_set(0);
    while (bus_iorq_get() == 0)
        z80_cycle();
    bus_data_control(READ);
    bus_cwait_set(1);
}

static void z80_do_interrupt()
{
    DEBUG("  sending interrupt signal");
    while (bus_m1_get() == 0)
        z80_cycle();

    bus_int_set(0);
    z80_cycle();
    bus_int_set(1);

    DEBUG("    waiting for IORQ to go low");
    while (bus_iorq_get() != 0)
        z80_cycle();
    z80_manage_iorq();
}

Z80_StepResult z80_single_step()
{
    DEBUG("-----------------");
    DEBUG("Z80 starting step");

    z80.updated = false;

    bus_busrq_set(1);   // make sure we're not requesting the bus

    if (z80.has_next_interrupt) {
        z80_do_interrupt();
        z80.has_next_interrupt = false;
    }

    z80_cycle();

    uint8_t i = 0;
    DEBUG("  cycling until M1");
    while (bus_m1_get() != 0) {
        z80_cycle();

        if (bus_iorq_get() == 0)
            z80_manage_iorq();

        if (i++ > 30)
            return Z_TOO_MANY_CYCLES;
    }

    z80.pc = bus_addr_get();
    DEBUG("  PC = %04X", z80.pc);

    z80.last_op = z80.next_op;
    z80.next_op = bus_data_get();

    z80_cycle();

    // when calling a Z80 combined instruction, a second step must be made
    static uint8_t previous_instruction = 0x00;
    bool combined_instruction = (previous_instruction == 0xcb || previous_instruction == 0xdd || previous_instruction == 0xed || previous_instruction == 0xfd);
    previous_instruction = bus_data_get();
    if (combined_instruction) {
        DEBUG("  combined instruction detected, doing another cycle");
        return z80_single_step();
    }

    DEBUG("Z80 step done");
    return Z_OK;
}

void z80_set_next_interrupt(uint8_t number)
{
    z80.has_next_interrupt = true;
    z80.next_interrupt = number;
}

Z80_StepResult z80_full_step()
{
    // call NMI subroutine
    bus_nmi_set(0);
    z80_single_step();
    z80_single_step();
    bus_nmi_set(1);

    // run NMI subroutine until it reaches 'retn'
    for (;;) {
        z80_single_step();
        if (z80.pc == 0x7f)
        // if (z80.last_op == 0xed && z80.next_op == 0x45) // retn
            break;
    }

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

    return Z_OK;
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