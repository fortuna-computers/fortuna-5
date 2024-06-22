#include "run.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "bus.h"
#include "io.h"

void run_init()
{
}

void run_step()
{
    uint8_t port;
    uint8_t data;

    if (bus_iorq_get() == 0) {

        MemPins mp = bus_mem_get();
        if (mp.wr == 0) {
            port = bus_addr_get() & 0xff;
            data = bus_data_get();
            io_out(port, data, false);
        } else if (mp.rd == 0) {
            port = bus_addr_get() & 0xff;
            data = io_in(port, false);
            bus_data_control(WRITE);
            bus_data_set(data);
        }
        // TODO - handle interrupt

        bus_cwait_set(0);
        while (bus_iorq_get() == 0)
            ;
        bus_data_control(READ);
        bus_cwait_set(1);
    }
}

/*
ISR(USART0_RX_vect) {
    io_last_key = UDR0;
}
 */