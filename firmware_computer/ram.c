#include "ram.h"

#include <avr/cpufunc.h>

#include "bus.h"

void ram_set(uint16_t pos, uint8_t data)
{
    // TODO - check for Z80 bus

    // check if data is already set
    uint8_t current_data = ram_get(pos);
    if (current_data == data)
        return;

    // set pin direction
    bus_mem_control(WRITE);
    bus_addr_control(WRITE);
    bus_data_control(WRITE);

    // set WE pin and wait
    bus_mem_write((MemPins) { .mreq = 0, .wr = 0, .rd = 1 });
    _NOP();

    // release pins
    bus_mem_control(READ);
    bus_addr_control(READ);
    bus_data_control(READ);
}

uint8_t ram_get(uint16_t pos)
{
    // TODO - check for Z80 bus

    bus_addr_control(WRITE);
    bus_mem_control(WRITE);

    bus_addr_write(pos);
    bus_mem_write((MemPins) { .mreq = 0, .wr = 1, .rd = 0 });

    _NOP();

    uint8_t data = bus_data_read();

    bus_mem_control(READ);
    bus_addr_control(READ);

    return data;
}