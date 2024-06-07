#include "io.h"

#include "libfdbg-server.h"
#include "uart.h"
#include "debugger.h"

void io_out(uint8_t port, uint8_t data, bool from_debugger)
{
    switch (port) {
        case 0x3: {   // write to UART
            if (from_debugger) {
                char text[2] = { (char) data, 0 };
                fdbg_server_terminal_print(debugger_server(), text);
                break;
            } else {
                uart_write_byte(data);
            }
        }
    }
}

uint8_t io_in(uint8_t port, bool from_debugger)
{
    switch (port) {
        case 0x3:
            return 0x42;
    }
    return 0;
}