#include <avr/interrupt.h>
#include <util/delay.h>

#include "libfdbg-server.h"
#include "uart.h"
#include "ram.h"
#include "bus.h"
#include "z80.h"

static FdbgServer server_;
static uint16_t pc = 0;

static fdbg_ComputerStatus get_computer_status(FdbgServer* server)
{
    (void) server;

    fdbg_ComputerStatus status = fdbg_ComputerStatus_init_zero;
    status.pc = pc;
    return status;
}

static void reset(FdbgServer* server)
{
    (void) server;

    pc = z80_reset();
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

    if (full)
        pc = z80_full_step();
    else
        pc = z80_single_step();
    return pc;
}

static bool next_instruction(FdbgServer* server, ADDR_TYPE* addr)
{
    uint8_t sz = z80_next_instruction_size();
    if (sz == 0) {
        return false;
    } else {
        *addr = pc + sz;
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

static uint16_t read_byte_async(FdbgServer* server)
{
    (void) server;
    uint16_t r = uart_read_byte_async();
    if (r == (uint16_t) -1)
        return SERIAL_NO_DATA;
    return (uint8_t) r;
}

static void write_byte(FdbgServer* server, uint8_t data)
{
    (void) server;
    uart_write_byte(data);
}

int main()
{
    bus_init();
    uart_init();
    z80_init();

    /*
    for (;;) {
        uint16_t c = uart_read_byte_async();
        if (c != SERIAL_NO_DATA)
            uart_write_byte(c);
    }
     */

    FdbgServerIOCallbacks cb = {
            .write_byte = write_byte,
            .read_byte_async = read_byte_async,
    };
    fdbg_server_init(&server_, 0x6ab9, cb);

    FdbgServerEvents events = {
            .get_computer_status = get_computer_status,
            .reset = reset,
            .step = step,
            .write_memory = write_memory,
            .read_memory = read_memory,
            .next_instruction = next_instruction,
            .cycle = cycle,
            // .on_keypress = on_keypress,
    };
    for (;;)
        fdbg_server_next(&server_, &events);
}
