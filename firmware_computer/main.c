#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"
#include "bus.h"
#include "z80.h"
#include "run.h"
#include "debugger.h"
#include "init.h"
#include "ram.h"
#include "debug.h"

int main()
{
    bus_init();
    uart_init();
    z80_init();
    run_init();
    debugger_init();
    debug_init();
    debug_print("Hello world!\n");

    initialize();

    sei();

    for (;;) {
        if (run_state == R_RUN)
            run_step();   // TODO - improve performance?
        else
            debugger_step();
    }
}
