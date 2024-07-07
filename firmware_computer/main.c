#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "uart.h"
#include "bus.h"
#include "z80.h"
#include "run.h"
#include "debugger.h"
#include "init.h"
#include "ram.h"
#include "io.h"
#include "debug.h"
#include "sdcard.h"
#include "spi.h"
#include "timer.h"

typedef enum { R_DEBUG, R_RUN } RunState;
volatile RunState run_state = R_DEBUG;

static void switch_check_init()
{
    // setup timer to check if pushbutton was pressed (every 250ms - 4 Hz)
    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3 = 0;
    OCR3A = 15624;            // 4 Hz (16000000/((15624+1)*256))
    TCCR3B |= (1 << WGM32);   // CTC
    TCCR3B |= (1 << CS32);    // Prescaler 256
    TIMSK3 |= (1 << OCIE3A);  // Output Compare Match A Interrupt Enable
}

int main()
{
    bus_init();
    bus_init_led_set(1);

    uart_init();
    z80_init();
    debugger_init();
    debug_init();
    spi_init();

    // sdcard_init();   // TODO - check result and do something about it

    switch_check_init();

    DEBUG("Microcontroller initialized.");

    initialize();   // write ROM to RAM

    DEBUG("Computer initialized.");

    bus_init_led_set(0);
    sei();

    for (;;) {
        if (run_state == R_RUN)
            run_step();   // TODO - improve performance?
        else
            debugger_step();
    }
}

ISR(TIMER3_COMPA_vect)   // called every 250ms to check state of the switch
{
    if (run_state == R_DEBUG && bus_sckl_ena_get() == 0) {  // run
        bus_clock_led_set(1);
        run_state = R_RUN;
        run_activate();

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
