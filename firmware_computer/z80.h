#ifndef Z80_H_
#define Z80_H_

#include <stdbool.h>
#include <stdint.h>

void     z80_init();

void     z80_cycle();
void     z80_release_bus();
uint16_t z80_reset();
uint16_t z80_single_step();
uint16_t z80_full_step();
uint8_t  z80_next_instruction_size();

#endif