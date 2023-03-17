#ifndef UART_H
#define UART_H

#include <avr/io.h>

char uart_rx(void);
void uart_print_twi_status();
void uart_printstr(const char *str);
void uart_printbyte(uint8_t byte);
void uart_tx(char c);
void uart_init();
void print_hex_value(unsigned char c);

#endif
