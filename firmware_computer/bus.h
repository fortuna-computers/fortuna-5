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

void     bus_set_reset(bool v);

void     bus_data_control(ControlMode mode);
uint8_t  bus_data_get();
void     bus_data_set(uint8_t data);

void     bus_addr_control(ControlMode mode);
uint16_t bus_addr_get();
void     bus_addr_set(uint16_t addr);

void     bus_mem_control(ControlMode mode);
MemPins  bus_mem_get();
void     bus_mem_set(MemPins pins);

#endif