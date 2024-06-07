#ifndef IO_H_
#define IO_H_

#include <stdbool.h>
#include <stdint.h>

uint8_t io_in(uint8_t port, bool from_debugger);
void    io_out(uint8_t port, uint8_t data, bool from_debugger);

#endif