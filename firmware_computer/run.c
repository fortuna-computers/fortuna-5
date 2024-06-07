#include "run.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "bus.h"

RunState run_state;

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
}

void run_step()
{
}

ISR(TIMER1_COMPA_vect)
{
    if (bus_sckl_ena_get() == 0) {  // debugger
        bus_clock_led_set(1);

    } else {  // run
        bus_clock_led_set(0);
    }
}