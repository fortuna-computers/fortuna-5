#include "io.h"

#include "libfdbg-server.h"

extern FdbgServer* server_;

void io_out(uint8_t port, uint8_t data)
{
    fdbg_server_terminal_print(server_, "X");
    /*
    switch (port) {
        case 0x3: {   // write to UART
            char text[2] = { (char) data, 0 };
            fdbg_server_terminal_print(server_, text);
            break;
        }
    }*/
}