#include <avr/interrupt.h>
#include <util/delay.h>

#include "libfdbg-server.h"
#include "uart.h"

FdbgServer server_;

static fdbg_ComputerStatus get_computer_status(FdbgServer* server)
{
    fdbg_ComputerStatus status = fdbg_ComputerStatus_init_zero;
    status.pc = 0;
    return status;
}

static void reset(FdbgServer* server)
{
}

static uint64_t step(FdbgServer* server, bool full, fdbg_Status* status)
{
    return 0;
}

static uint64_t next_instruction(FdbgServer* server)
{
    return 0;
}

static bool write_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed)
{
    return true;
}

static void read_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t sz, uint8_t* out_data)
{
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
    uart_init();

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
            // .on_keypress = on_keypress,
    };
    for (;;)
        fdbg_server_next(&server_, &events);
}
