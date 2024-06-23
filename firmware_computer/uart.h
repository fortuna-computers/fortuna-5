#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void     uart_init();
void     uart_write_byte(uint8_t byte);
uint16_t uart_read_byte_async();
void     uart_add_to_queue(uint8_t byte);

#endif