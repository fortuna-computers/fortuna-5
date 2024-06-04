#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>
#include <stdint.h>

void z80_init();
void z80_cycle();
void z80_release_bus();
uint16_t z80_reset();
uint16_t z80_single_step();

#endif