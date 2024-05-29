#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void uart_init()
{
    // set speed
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // set config
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);   // async mode
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);     // Enable Receiver and Transmitter

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
}

void uart_write_byte(uint8_t byte)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = byte;
}

uint16_t uart_read_byte_async()
{
    if (!bit_is_set(UCSR0A, RXC0))
        return (uint16_t) -1;
    return UDR0;
}