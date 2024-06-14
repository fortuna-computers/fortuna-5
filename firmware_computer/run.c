#include "run.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "bus.h"
#include "io.h"

volatile RunState run_state;

void run_init()
{
    run_state = R_DEBUG;

    // setup timer to check if pushbutton was pressed (every 250ms - 4 Hz)
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // 4 Hz (16000000/((15624+1)*256))
    OCR1A = 15624;
    // CTC
    TCCR1B |= (1 << WGM12);
    // Prescaler 256
    TCCR1B |= (1 << CS12);
    // Output Compare Match A Interrupt Enable
    TIMSK1 |= (1 << OCIE1A);

    // setup interrupt to listen UART
    UCSR0B |= (1<<RXCIE0);
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

ISR(TIMER1_COMPA_vect)
{
    if (run_state == R_DEBUG && bus_sckl_ena_get() == 0) {  // run
        bus_clock_led_set(1);
        run_state = R_RUN;

    } else if (run_state == R_RUN && bus_sckl_ena_get() == 1) {  // debugger
        bus_clock_led_set(0);
        run_state = R_DEBUG;

    }
}

ISR(USART0_RX_vect) {
    io_last_key = UDR0;
}