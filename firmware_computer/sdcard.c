#include "sdcard.h"

#include "spi.h"

#include "debug.h"

SD_DEV dev[1];
uint8_t sd_buffer[512];

bool sdcard_init()
{
    SDRESULTS r = SD_Init(dev);

    if (r == SD_OK) {
        DEBUG("SDCard initialized.");
        return true;
    } else {
        DEBUG("SDCard NOT initialized.");
        return false;
    }
}

//
// functions required by ulibSD
//

void SPI_Init (void)
{
    // it's already initialized
}

BYTE SPI_RW (BYTE d)
{
    return spi_xchg(d);
}

void SPI_Release (void)
{
    // do nothing
}

void SPI_CS_Low (void)
{
    spi_cs(SD_CS, 0);
}

void SPI_CS_High (void)
{
    spi_cs(SD_CS, 1);
}

void SPI_Freq_High (void)
{
    spi_speed(SPD_HIGH);
}

void SPI_Freq_Low (void)
{
    spi_speed(SPD_LOW);
}

void SPI_Timer_On (WORD ms)
{
}

BOOL SPI_Timer_Status (void)
{
}

void SPI_Timer_Off (void)
{
}

