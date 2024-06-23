#include "uart.h"
#include "libfdbg-server.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/setbaud.h>

#define BUF_SZ 256

static volatile uint8_t  queue[BUF_SZ] = {0};   // circular queue
static volatile uint16_t queue_start = 0;
static volatile uint16_t queue_end   = 0;

void uart_init()
{
    // set speed
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // set config
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);   // 8-bit
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);     // Enable Receiver and Transmitter

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    UCSR0B |= (1<<RXCIE0);   // setup RX interrupt
}

void uart_write_byte(uint8_t byte)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = byte;
}

uint16_t uart_read_byte_async()
{
    if (queue_start == queue_end)
        return SERIAL_NO_DATA;

    uint8_t r = queue[queue_start++];
    if (queue_start == BUF_SZ)
        queue_start = 0;
    return r;
}

void uart_add_to_queue(uint8_t byte)
{
    queue[queue_end++] = byte;
    if (queue_end == BUF_SZ)
        queue_end = 0;
}