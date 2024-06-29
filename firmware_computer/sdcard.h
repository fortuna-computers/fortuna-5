#ifndef SDCARD_H_
#define SDCARD_H_

#include <stdbool.h>
#include <stdint.h>

#include "ulibSD/sd_io.h"

extern uint8_t sd_buffer[512];

bool sdcard_init();

#endif