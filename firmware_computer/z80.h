#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>
#include <stdint.h>
#include "common.h"

void    z80_init();

void    z80_cycle();
void    z80_release_bus();
void    z80_reset();
void    z80_single_step();
void    z80_full_step();
uint8_t z80_next_instruction_size();

typedef struct {
    uint16_t pc;
    uint8_t  next_op;
    uint16_t af, bc, de, hl, afx, bcx, dex, hlx, ix, iy, sp;
    uint8_t  i;
    uint8_t  stack[STACK_SZ];
    bool     updated;
} Z80_Status;

extern volatile Z80_Status z80;

#endif