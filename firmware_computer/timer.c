#include "timer.h"

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

// code based on https://github.com/monoclecat/avr-millis-function/

static volatile uint32_t current = 0;

void timer_reset()
{
    current = 0;

    unsigned long ctc_match_overflow;
    ctc_match_overflow = ((F_CPU / 1000) / 8); //when timer1 is this value, 1ms has passed

    // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
    TCCR1B |= (1 << WGM12) | (1 << CS11);

    // high byte first, then low byte
    OCR1AH = (ctc_match_overflow >> 8);
    OCR1AL = ctc_match_overflow;

    // Enable the compare match interrupt
    TIMSK1 |= (1 << OCIE1A);
}

uint32_t timer_current()
{
    unsigned long millis_return;

    // Ensure this cannot be disrupted
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        millis_return = current;
    }
    return millis_return;
}

void timer_stop()
{
}

ISR(TIMER1_COMPA_vect)
{
    ++current;
}