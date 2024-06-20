#include "debug.h"

#include <stddef.h>

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

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

#endif
}

void debug_print(const char* str)
{
#if DEBUG_ENABLE == 1
    for (size_t i = 0; str[i]; ++i) {
        loop_until_bit_is_set(UCSR3A, UDRE3);
        UDR3 = str[i];
    }
#endif
}