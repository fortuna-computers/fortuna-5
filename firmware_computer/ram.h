#include <stdint.h>

#ifndef RAM_H_
#define RAM_H_

void    ram_set(uint16_t pos, uint8_t data);
uint8_t ram_get(uint16_t pos);

#endif