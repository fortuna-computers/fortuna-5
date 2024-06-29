#ifndef DEBUG_H_
#define DEBUG_H_

#include <avr/pgmspace.h>

#define DEBUG_ENABLE 0

void debug_init();
void debug_printf(PGM_P fmt, ...);
void debug_putc(char c);

#if DEBUG_ENABLE == 1
#  define DEBUG(fmt, ...) debug_printf(PSTR(fmt), ##__VA_ARGS__)
#else
#  define DEBUG(fmt, ...) {}
#endif

#endif