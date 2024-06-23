#include "run.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "bus.h"
#include "io.h"

void run_activate()
{
    bus_reset_set(1);
    bus_nmi_set(1);
    bus_int_set(1);
    bus_busrq_set(1);
    bus_cwait_set(1);

    bus_data_control(READ);
    bus_addr_control(READ);
    bus_mem_control(READ);
}

void run_step()
{
    uint8_t port;
    uint8_t data;

    if (bus_iorq_get() == 0) {

        MemPins mp = bus_mem_get();
        if (mp.wr == 0 && mp.rd == 1) {
            port = bus_addr_get() & 0xff;
            data = PINA;    // read DATA bus
            io_out(port, data, false);

        } else if (mp.rd == 0 && mp.wr == 1) {
            port = bus_addr_get() & 0xff;
            data = io_in(port, false);
            DDRA = 0xff;    // control DATA bus
            PORTA = data;   // write to DATA bus
        }

        // TODO - handle interrupt

        PORTH &= ~(1<<PH1);   // CWAIT = 0
        DDRA = 0;             // release DATA bus
        PORTH |= 1<<PH1;      // CWAIT = 1
    }
}