#include "libfdbg-server.h"

FdbgServer server;

int main()
{
    FdbgServerIOCallbacks cb = {
    };
    fdbg_server_init(&server, 0x6ab9, cb);

    FdbgServerEvents events = {
    };
    for (;;)
        fdbg_server_next(&server, &events);
}
