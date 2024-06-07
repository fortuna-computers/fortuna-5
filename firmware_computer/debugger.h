#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include "libfdbg-server.h"

void        debugger_init();
void        debugger_step();
FdbgServer* debugger_server();

#endif