#include "main.h"

ISR(TWI_vect)
{
	uart_printstr("TWI interrupt\r\n");
	uart_print_twi_status();
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

ISR(INT0_vect)
{
	uart_printstr("BUTTON PRESSED\n\r");
	uart_printstr("SENDING GLOBAL CALL\n\r");
	i2c_send_byte(0x0, 0x1);
}

int main()
{
	uart_init();
	i2c_init();

	DDRD &= ~(1 << BUTTON);
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();

	for(;;)
	{
		_delay_ms(1000);
		uart_printstr("looping\r\n");
	}
}
