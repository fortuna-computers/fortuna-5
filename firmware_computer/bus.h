#ifndef BUS_H_
#define BUS_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    READ, WRITE
} ControlMode;

typedef struct {
    bool mreq : 1;
    bool wr   : 1;
    bool rd   : 1;
} MemPins;

void     bus_init();

void     bus_data_control(ControlMode mode);
uint8_t  bus_data_get();
void     bus_data_set(uint8_t data);

void     bus_addr_control(ControlMode mode);
uint16_t bus_addr_get();
void     bus_addr_set(uint16_t addr);

void     bus_mem_control(ControlMode mode);
MemPins  bus_mem_get();
void     bus_mem_set(MemPins pins);

bool     bus_busak_get();
bool     bus_m1_get();
bool     bus_wait_get();
bool     bus_halt_get();
bool     bus_reset_get();
bool     bus_nmi_get();
bool     bus_int_get();
bool     bus_iorq_get();
bool     bus_busrq_get();

void     bus_reset_set(bool v);
void     bus_nmi_set(bool v);
void     bus_int_set(bool v);
void     bus_busrq_set(bool v);
void     bus_clk_set(bool v);
void     bus_wait_set(bool v);

#endif