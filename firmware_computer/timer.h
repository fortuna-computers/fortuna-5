#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void     timer_reset();
uint32_t timer_current();
void     timer_stop();

#endif