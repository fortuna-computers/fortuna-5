#include "bus.h"

#include <avr/io.h>

#define SET(port, pin, value) { if (value) PORT ## port = (1<<pin); else PORT ## port &= ~(1<<pin); }

void bus_init()
{
    DDRL = 0b01110101;   // SIMCLK, A16, A17, A18, RESET
    DDRG = (1<<DDG2);    // SCLK_ENA
    DDRB = (1<<DDB0) | (1<<DDB4) | (1<<DDB5);   // CS0..2
    DDRC = (1<<DDC0) | (1<<DDC1) | (1<<DDC5) | (1<<DDC6);   // NMI, INT, BUSRQ, CWAIT
}

void bus_set_reset(bool v)
{
    SET(H, 6, v)
}

void bus_data_control(ControlMode mode)
{
    DDRA = (mode == READ) ? 0x0 : 0xff;
}

uint8_t bus_data_read()
{
    return PINA;
}

void bus_data_write(uint8_t data)
{
    PORTA = data;
}

void bus_addr_control(ControlMode mode)
{
    DDRF = (mode == READ) ? 0x0 : 0xff;
    DDRK = (mode == READ) ? 0x0 : 0xff;
}

uint16_t bus_addr_read()
{
    return (((uint16_t) PINK) << 8) | PINF;
}

void bus_addr_write(uint16_t addr)
{
    PINK = (addr >> 8);
    PINF = (uint8_t) addr;
}

void bus_mem_control(ControlMode mode)
{
    if (mode == WRITE) {
        DDRC |= (1<<DDC2) | (1<<DDC3) | (1<<DDC4);
        bus_mem_write((MemPins) { 1, 1, 1 });
    } else {
        DDRC &= ~(1<<DDC2) | ~(1<<DDC3) | ~(1<<DDC4);
        bus_mem_write((MemPins) { 1, 1, 1 });
    }
}

MemPins bus_mem_read()
{
    uint8_t c = PINC;
    return (MemPins) {
        .mreq = c & (1<<PINC4),
        .wr = c & (1<<PINC3),
        .rd = c & (1<<PINC2),
    };
}

void bus_mem_write(MemPins pins)
{
    SET(C, 2, pins.rd)
    SET(C, 3, pins.wr)
    SET(C, 4, pins.mreq)
}