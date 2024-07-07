#ifndef DEBUG_H_
#define DEBUG_H_

#include <avr/pgmspace.h>

#define DEBUG_ENABLE 0
#define DEBUG_SPI    0

#define ANSI_GREEN "\e[1;32m"
#define ANSI_RED   "\e[0;31m"
#define ANSI_RESET "\e[0m"

void debug_init();
void debug_printf(PGM_P fmt, ...);   // do not use directly
void debug_putc(char c);

#if DEBUG_ENABLE == 1
#  define DEBUG(fmt, ...)  debug_printf(PSTR(fmt "\r\n"), ##__VA_ARGS__)
#  define DEBUGN(fmt, ...) debug_printf(PSTR(fmt), ##__VA_ARGS__)
#else
#  define DEBUG(fmt, ...)  {}
#  define DEBUGN(fmt, ...) {}
#endif

#endif