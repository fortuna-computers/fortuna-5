#include "libfdbg-server.h"
#include "uart.h"
#include "bus.h"
#include "z80.h"
#include "run.h"
#include "debugger.h"

int main()
{
    bus_init();
    uart_init();
    z80_init();
    run_init();
    debugger_init();

    for (;;)
        debugger_step();
}
