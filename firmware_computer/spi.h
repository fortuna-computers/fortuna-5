#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum { SD_CS, CS1, CS2 } SPIPort;
typedef enum { SPD_LOW, SPD_HIGH } SPISpeed;

void    spi_init();
void    spi_speed(SPISpeed speed);

void    spi_cs(SPIPort port, bool value);
uint8_t spi_xchg(uint8_t data);

#endif