#include "debugger.h"
#include "libfdbg-server.h"

#include "z80.h"
#include "bus.h"
#include "ram.h"
#include "uart.h"
#include "io.h"

static FdbgServer server_;

static fdbg_ComputerStatus get_computer_status(FdbgServer* server)
{
    (void) server;

    fdbg_ComputerStatus status = fdbg_ComputerStatus_init_zero;
    status.pc = z80.pc;
    if (z80.updated) {
        status.registers_count = 12;
        status.registers[0] = z80.af;
        status.registers[1] = z80.bc;
        status.registers[2] = z80.de;
        status.registers[3] = z80.hl;
        status.registers[4] = z80.afx;
        status.registers[5] = z80.bcx;
        status.registers[6] = z80.dex;
        status.registers[7] = z80.hlx;
        status.registers[8] = z80.ix;
        status.registers[9] = z80.iy;
        status.registers[10] = z80.sp;
        status.registers[11] = z80.i;

        status.flags_count = 6;
        status.flags[0] = z80.af & (1 >> 7);
        status.flags[1] = z80.af & (1 >> 6);
        status.flags[2] = z80.af & (1 >> 4);
        status.flags[3] = z80.af & (1 >> 2);
        status.flags[4] = z80.af & (1 >> 1);
        status.flags[5] = z80.af & (1 >> 0);

        status.stack.size = STACK_SZ;
        for (size_t i = 0; i < STACK_SZ; ++i)
            status.stack.bytes[i] = z80.stack[i];
    }
    return status;
}

static void reset(FdbgServer* server)
{
    (void) server;

    z80_reset();
    z80_single_step(true);
}

static fdbg_CycleResponse cycle(FdbgServer* server)
{
    z80_cycle();

    fdbg_CycleResponse cycle = fdbg_CycleResponse_init_zero;

    cycle.bytes_count = 2;
    if (bus_iorq_get() == 0 || bus_mem_get().mreq == 0) {
        cycle.bytes[0].has = true;
        cycle.bytes[1].has = true;
        cycle.bytes[0].value = bus_addr_get();
        cycle.bytes[1].value = bus_data_get();
    } else {
        cycle.bytes[0].has = false;
        cycle.bytes[1].has = false;
    }

    MemPins mp = bus_mem_get();
    bool m1 = bus_m1_get();

    cycle.bits_count = 12;
    cycle.bits[0] = bus_nmi_get();
    cycle.bits[1] = bus_int_get();
    cycle.bits[2] = m1;
    cycle.bits[3] = bus_halt_get();
    cycle.bits[4] = bus_wait_get();
    cycle.bits[5] = mp.mreq;
    cycle.bits[6] = mp.wr;
    cycle.bits[7] = mp.rd;
    cycle.bits[8] = bus_iorq_get();
    cycle.bits[9] = bus_busrq_get();
    cycle.bits[10] = bus_busak_get();
    cycle.bits[11] = bus_reset_get();

    if (mp.mreq == 0 && m1 == 0)
        cycle.pc = bus_addr_get();

    return cycle;
}

static uint64_t step(FdbgServer* server, bool full, fdbg_Status* status)
{
    (void) server;

    Z80_StepResult result;
    if (full)
        result = z80_full_step();
    else
        result = z80_single_step(true);

    if (result == Z_TOO_MANY_CYCLES)
        *status = fdbg_Status_INFINITE_LOOP;

    return z80.pc;
}

static bool next_instruction(FdbgServer* server, ADDR_TYPE* addr)
{
    uint8_t sz = z80_next_instruction_size();
    if (sz == 0) {
        return false;
    } else {
        *addr = z80.pc + sz;
        return true;
    }
}

static bool write_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed)
{
    (void) server;
    (void) nr;
    (void) first_failed;

    for (size_t i = 0; i < sz; ++i)
        ram_set(pos + i, data[i]);

    return true;
}

static void read_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t sz, uint8_t* out_data)
{
    (void) server;
    (void) nr;

    for (size_t i = 0; i < sz; ++i)
        out_data[i] = ram_get(pos + i);
}

static void interrupt(FdbgServer* server, uint64_t number)
{
    z80_set_next_interrupt(number);
}

static uint16_t read_byte_async(FdbgServer* server)
{
    (void) server;
    uint16_t r = uart_read_byte_async();
    if (r == (uint16_t) -1)
        return SERIAL_NO_DATA;
    return (uint8_t) r;
}

static uint8_t read_byte_sync(FdbgServer* server)
{
    (void) server;
    return uart_read_byte_sync();
}

static void write_byte(FdbgServer* server, uint8_t data)
{
    (void) server;
    uart_write_byte(data);
}

static void on_keypress(FdbgServer* server, const char* key)
{
    (void) server;

    io_last_key = key[0];
    // TODO - fire interrupt
}

static FdbgServerEvents events = {
        .get_computer_status = get_computer_status,
        .reset = reset,
        .step = step,
        .write_memory = write_memory,
        .read_memory = read_memory,
        .next_instruction = next_instruction,
        .cycle = cycle,
        .interrupt = interrupt,
        .on_keypress = on_keypress,
};

void debugger_init()
{
    FdbgServerIOCallbacks cb = {
            .write_byte = write_byte,
            .read_byte_async = read_byte_async,
            .read_byte_sync = read_byte_sync,
    };
    fdbg_server_init(&server_, 0x6ab9, cb);
}

void debugger_step()
{
    fdbg_server_next(&server_, &events);
}

FdbgServer* debugger_server()
{
    return &server_;
}