#include "bus.h"

#include <avr/io.h>

#define SET(port, pin, value) { if (value) { PORT ## port |= (1<<pin); } else { PORT ## port &= ~(1<<pin); } }

void bus_init()
{
    DDRL = 0b10101110;   // SIMCLK, A16, A17, A18, RESET
    DDRG = (1<<DDG0) | (1<<DDG2);    // NMI, SCLK_ENA
    DDRB = (1<<DDB0) | (1<<DDB4) | (1<<DDB5);   // CS0..2
    DDRC = (1<<DDC1) | (1<<DDC5) | (1<<DDC6);   // INT, BUSRQ, CWAIT
}

void bus_data_control(ControlMode mode)
{
    DDRA = (mode == READ) ? 0x0 : 0xff;
    if (mode == READ)
        DDRA = 0;
}

uint8_t bus_data_get()
{
    return PINA;
}

void bus_data_set(uint8_t data)
{
    PORTA = data;
}

void bus_addr_control(ControlMode mode)
{
    DDRF = (mode == READ) ? 0x0 : 0xff;
    DDRK = (mode == READ) ? 0x0 : 0xff;
    if (mode == READ) {
        PORTF = 0;
        PORTK = 0;
    }
}

uint16_t bus_addr_get()
{
    return (((uint16_t) PINK) << 8) | PINF;
}

void bus_addr_set(uint16_t addr)
{
    PORTK = (uint8_t) (addr >> 8);
    PORTF = (uint8_t) addr;
}

void bus_mem_control(ControlMode mode)
{
    if (mode == WRITE) {
        DDRC |= (1<<DDC2) | (1<<DDC3) | (1<<DDC4);
        bus_mem_set((MemPins) {1, 1, 1});
    } else {
        DDRC &= ~((1<<DDC2) | (1<<DDC3) | (1<<DDC4));
        bus_mem_set((MemPins) {1, 1, 1});
    }
}

MemPins bus_mem_get()
{
    uint8_t c = PINC;
    return (MemPins) {
        .mreq = (c & (1<<PINC4)) != 0,
        .wr = (c & (1<<PINC3)) != 0,
        .rd = (c & (1<<PINC2)) != 0,
    };
}

void bus_mem_set(MemPins pins)
{
    SET(C, 2, pins.rd)
    SET(C, 3, pins.wr)
    SET(C, 4, pins.mreq)
}

bool bus_busak_get() { return PINH & (1<<PINH0); }
bool bus_m1_get()    { return PIND & (1<<PIND1); }
bool bus_wait_get()  { return PIND & (1<<PIND2); }
bool bus_halt_get()  { return PIND & (1<<PIND3); }
bool bus_reset_get() { return PINL & (1<<PINL7); }
bool bus_nmi_get()   { return PING & (1<<PING0); }
bool bus_int_get()   { return PINC & (1<<PINC1); }
bool bus_iorq_get()  { return PIND & (1<<PIND0); }
bool bus_busrq_get() { return PINC & (1<<PINC5); }

void bus_reset_set(bool v) { SET(L, 7, v) }
void bus_nmi_set(bool v)   { SET(G, 0, v) }
void bus_int_set(bool v)   { SET(C, 1, v) }
void bus_clk_set(bool v)   { SET(L, 1, v) }
void bus_busrq_set(bool v) { SET(C, 5, v) }
void bus_wait_set(bool v)  { SET(D, 2, v) }
