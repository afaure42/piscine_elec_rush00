#include "uart.h"

void uart_init()
{
	//set baudrate
	UBRR0H = UART_BAUD_SETTING >> 8;
	UBRR0L = UART_BAUD_SETTING;

	//enable transmitter and receiver
	UCSR0B = 1 << TXEN0 | 1 << RXEN0;

	//set frame format (8data, no parity, 1 stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

}

void uart_tx(char c)
{
	//writing data to send register
	while(((UCSR0A >> UDRE0) & 1) == 0);
	UDR0 = c;
}

void uart_printbyte(uint8_t byte)
{
	for(uint8_t i = 8; i > 0; i--)
		uart_tx(((byte >> (i - 1)) & 1) + '0');
}

void uart_printstr(const char *str)
{
	while(*str)
	{
		uart_tx(*str);
		str++;
	}
}

void uart_putnbr(int32_t number)
{
	if (number < 0)
	{
		number *= -1;
		uart_tx('-');
	}
	if (number >= 10)
		uart_putnbr(number / 10);
	uart_tx(number % 10 + '0');
}
