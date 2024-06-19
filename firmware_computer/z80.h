#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>
#include <stdint.h>
#include "sizes.h"

typedef enum {
    Z_OK, Z_TOO_MANY_CYCLES,
} Z80_StepResult;

void    z80_init();

void    z80_cycle();
void    z80_release_bus();
void    z80_reset();
uint8_t z80_next_instruction_size();

Z80_StepResult z80_single_step();
Z80_StepResult z80_full_step();

void    z80_set_next_interrupt(uint8_t number);

typedef struct {
    uint16_t pc;
    uint8_t  next_op;
    uint8_t  last_op;
    uint16_t af, bc, de, hl, afx, bcx, dex, hlx, ix, iy, sp;
    uint8_t  i;
    uint8_t  stack[STACK_SZ];
    uint8_t  next_interrupt;
    bool     updated;
    bool     has_next_interrupt;
} Z80_Status;

extern volatile Z80_Status z80;

#endif