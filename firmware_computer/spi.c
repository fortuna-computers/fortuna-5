#include "spi.h"

#include <avr/io.h>

void spi_init()
{
    DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB4) | (1<<DDB5);  // SD_CS, SCK, MOSI, CS1, CS2
    SPCR = (1<<MSTR) | (1<<SPE) | (1<<SPR1) | (1<<SPR0);   // speed f/64, master, SPI enable
    SPSR &= ~(1<<SPI2X);
}

void spi_speed(SPISpeed speed)
{
    if (speed == SPD_LOW) {
        SPCR = (1<<MSTR) | (1<<SPE) | (1<<SPR1) | (1<<SPR0);   // speed f/64, master, SPI enable
        SPSR &= ~(1<<SPI2X);
    } else {
        /*   TODO - disabled for now
        SPCR = (1<<SPE)|(1<<MSTR);
        SPSR = (1<<SPI2X);
         */
    }
}

void spi_cs(SPIPort port, bool value)
{
    switch (port) {
        case SD_CS:
            if (value)
                PORTB |= (1<<PB0);
            else
                PORTB &= ~(1<<PB0);
            break;
        case CS1:
            if (value)
                PORTB |= (1<<PB4);
            else
                PORTB &= ~(1<<PB4);
            break;
        case CS2:
            if (value)
                PORTB |= (1<<PB5);
            else
                PORTB &= ~(1<<PB5);
            break;
    }
}

uint8_t spi_xchg(uint8_t data)
{
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));  // wait for transmission
    return SPDR;
}