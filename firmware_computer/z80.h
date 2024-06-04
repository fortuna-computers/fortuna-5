#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>
#include <stdint.h>

void z80_init();
void z80_reset();
void z80_cycle();

#endif