#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include "i2c.h"

#define UART_BAUD_SETTING (((F_CPU / 8 / UART_BAUDRATE ) -1 ) / 2)

#define I2C_ADDR 0xF8

#define GAME_LOST	0xFF
#define GAME_WON	0x00

int main()
{
	uart_init();
	i2c_init();

	while (1)
	{
		//wait for ok signal

		// start countdown
		
		//if button pushed before end of countdown
			//start dead led
		//if button pushed after
			//send win signal
		//sending win signal
		i2c_send_byte(I2C_ADDR, GAME_WON);
		//if win signal received enable game lost blinking
		//maybe use a global variable and set it if
		//a win signal is received
	}
}
