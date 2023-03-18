#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "uart.h"

volatile int32_t timer_count = 0;

ISR(TIMER1_OVF_vect) {
	timer_count++;
	uart_putnbr(timer_count);
}

void start_game_timer(void) {
	//Start timer 1
	//Set prescaler to 1024 (16000000/1024 = 15625Hz) 
	//Set timer to CTC mode
	//Max value of 16bit timer is 65535 (65535/15625 = ~4.2 => use 4s to be precise)
	//Set OCR1A to 4 * 15625 = 62500
	//Enable interrupt on OCR1A
	TCNT1 = 0;

	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	TCCR1B |= (1 << CS12) | (1 << CS10);

	OCR1A = 62500;

	TIMSK1 |= (1 << OCIE1A);

	sei();
}

int main() {
	uart_init();
	start_game_timer();

	for(;;);
}