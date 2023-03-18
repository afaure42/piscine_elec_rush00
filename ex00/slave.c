#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "uart.h"
#include "i2c.h"

void stop_game_waiting();

volatile int32_t timer_count = 0;
volatile int32_t clicked_ms = 0;

//Timer 1 counted 4 secondes
ISR(TIMER1_COMPA_vect) {
	timer_count++;
}

//Button SW1 pressed
ISR(INT0_vect) {
	//Get timer value
	int32_t timer_ms = TCNT1 / 15.625;
	//Add stored timer value
	clicked_ms = timer_count * 4000 + timer_ms;
	stop_game_waiting();
}

void start_game_waiting(void) {
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

	//Setup button SW1 as input
	//Detect falling edge on INT0
	//Enable interrupt on INT0
	DDRD &= ~(1 << PD2);
	EICRA |= (1 << ISC01) | (0 << ISC00);
	EIMSK |= (1 << INT0);
}

void stop_game_waiting() {
	//Disable interrupt on INT0
	EIMSK &= ~(1 << INT0);
	//Disable interrupt on OCR1A
	TIMSK1 &= ~(1 << OCIE1A);
	//Stop timer 1
	TCCR1B = 0;
	timer_count = 0;
}

void send_status_to_master() {

}

int main() {
	uart_init();
	i2c_init_as_slave();
	start_game_waiting();

	sei();

	for(;;);
}