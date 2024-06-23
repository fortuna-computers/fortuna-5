#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"
#include "bus.h"
#include "z80.h"
#include "run.h"
#include "debugger.h"
#include "init.h"
#include "ram.h"
#include "io.h"
#include "debug.h"

typedef enum { R_DEBUG, R_RUN } RunState;
volatile RunState run_state = R_DEBUG;

static void timer_init()
{
    // setup timer to check if pushbutton was pressed (every 250ms - 4 Hz)
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 15624;            // 4 Hz (16000000/((15624+1)*256))
    TCCR1B |= (1 << WGM12);   // CTC
    TCCR1B |= (1 << CS12);    // Prescaler 256
    TIMSK1 |= (1 << OCIE1A);  // Output Compare Match A Interrupt Enable
}

int main()
{
    bus_init();
    uart_init();
    z80_init();
    run_init();
    debugger_init();
    debug_init();

    timer_init();

    DEBUG("Microcontroller initialized.");

    initialize();   // write ROM to RAM

    DEBUG("Computer initialized.");

    sei();

    for (;;) {
        if (run_state == R_RUN)
            run_step();   // TODO - improve performance?
        else
            debugger_step();
    }
}

ISR(TIMER1_COMPA_vect)   // called every 250ms to check state of the switch
{
    if (run_state == R_DEBUG && bus_sckl_ena_get() == 0) {  // run
        bus_clock_led_set(1);
        run_state = R_RUN;

    } else if (run_state == R_RUN && bus_sckl_ena_get() == 1) {  // debugger
        bus_clock_led_set(0);
        run_state = R_DEBUG;

    }
}

ISR(USART0_RX_vect)
{
    if (run_state == R_DEBUG)
        uart_add_to_queue(UDR0);
    else
        io_last_key = UDR0;
}
