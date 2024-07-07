#include "debug.h"

#include <stddef.h>
#include <stdio.h>

#include <avr/io.h>
#include <util/setbaud.h>

#define BUF_MAX_SZ 255

__attribute__((unused)) static void debug_print(const char* str)
{
#if DEBUG_ENABLE == 1
    for (size_t i = 0; str[i]; ++i) {
        loop_until_bit_is_set(UCSR3A, UDRE3);
        UDR3 = str[i];
    }
#endif
}

void debug_putc(char c)
{
#if DEBUG_ENABLE == 1
    loop_until_bit_is_set(UCSR3A, UDRE3);
    UDR3 = c;
#endif
}

void debug_init()
{
#if DEBUG_ENABLE == 1
    // set speed
    UBRR3H = UBRRH_VALUE;
    UBRR3L = UBRRL_VALUE;

    // set config
    UCSR3C = (1<<UCSZ31) | (1<<UCSZ30);   // 8-bit
    UCSR3B = (1<<RXEN3) | (1<<TXEN3);     // Enable Receiver and Transmitter

#if USE_2X
    UCSR3A |= (1 << U2X3);
#else
    UCSR3A &= ~(1 << U2X3);
#endif

    debug_print("\e[1;1H\e[2J");
#endif
}

void debug_printf(PGM_P fmt, ...)
{
#if DEBUG_ENABLE == 1
    va_list args;
    va_start(args, fmt);
    int sz = vsnprintf_P(NULL, 0, fmt, args) + 1;
    if (sz > BUF_MAX_SZ)
        sz = BUF_MAX_SZ;
    char buf[sz];
    vsnprintf_P(buf, sz, fmt, args);
    va_end(args);
    debug_print(buf);
#endif
}
