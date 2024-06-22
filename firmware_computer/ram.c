#include "ram.h"

#include <avr/cpufunc.h>
#include <util/delay.h>

#include "bus.h"
#include "z80.h"
#include "debug.h"

void ram_set(uint16_t pos, uint8_t data)
{
    z80_release_bus();

    // check if data is already set
    uint8_t current_data = ram_get(pos);
    if (current_data == data)
        return;

    // set pin direction
    bus_mem_control(WRITE);
    bus_addr_control(WRITE);
    bus_data_control(WRITE);

    // set WE pin and wait
    bus_addr_set(pos);
    bus_data_set(data);
    bus_mem_set((MemPins) {.mreq = 0, .wr = 0, .rd = 1});
    _NOP();

    // release pins
    bus_mem_control(READ);
    bus_addr_control(READ);
    bus_data_control(READ);

    // DEBUG("[%04X] = %02X", pos, data);
}

uint8_t ram_get(uint16_t pos)
{
    z80_release_bus();

    bus_addr_control(WRITE);
    bus_mem_control(WRITE);

    bus_addr_set(pos);
    bus_mem_set((MemPins) {.mreq = 0, .wr = 1, .rd = 0});

    _NOP();

    uint8_t data = bus_data_get();

    bus_mem_control(READ);
    bus_addr_control(READ);

    // DEBUG("%02X <- [%04X]", data, pos);

    return data;
}