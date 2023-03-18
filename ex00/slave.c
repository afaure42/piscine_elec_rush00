#include "slave.h"

void stop_game_waiting();

volatile int32_t timer_count = 0;
volatile int32_t clicked_ms = 0;
volatile int button_pressed = 0;
volatile slave_state current_slave_state = SLAVE_STATE_STARTING;

//Timer 1 counted 1 secondes
ISR(TIMER1_COMPA_vect) {
	timer_count++;
	if (timer_count == (TIMER - 1) / 4) {
		PORTB |= (1 << LED1);
	}
	if (timer_count == (TIMER - 1) / 2) {
		PORTB |= (1 << LED2);
	}
	if (timer_count == (TIMER - 1) * 3 / 4) {
		PORTB |= (1 << LED3);
	}
	if (timer_count == (TIMER - 1)) {
		PORTB |= (1 << LED4);
	}
	if (timer_count == TIMER) {
		PORTB &= ~(1 << LED1);
		PORTB &= ~(1 << LED2);
		PORTB &= ~(1 << LED3);
		PORTB &= ~(1 << LED4);
	}
}

//Button SW1 pressed
ISR(INT0_vect) {
	if (current_slave_state == SLAVE_STATE_STARTING) {
		button_pressed = 1;
		PORTD |= (1 << LED_G);
		PORTD &= ~(1 << LED_R);

		uart_printstr("Set user ready\r\n");

		//DEBUG
		_delay_ms(1000);
		command_start();
		return;
	} else if (current_slave_state == SLAVE_STATE_PLAYING && clicked_ms == 0) {
		//Get timer value
		int32_t timer_ms = TCNT1 / 15.625;
		//Add stored timer value
		clicked_ms = timer_count * 1000 + timer_ms;
		stop_game_waiting();
		PORTB &= ~(1 << LED1);
		PORTB &= ~(1 << LED2);
		PORTB &= ~(1 << LED3);
		PORTB &= ~(1 << LED4);

		uart_printstr("Button pressed after ");
		uart_putnbr(clicked_ms);
		uart_printstr("ms\r\n");

		//DEBUG
		_delay_ms(1000);
		command_win();
		_delay_ms(2000);
		command_reset();
	}
}

void start_game_waiting(void) {
	//Start timer 1
	//Set prescaler to 1024 (16000000/1024 = 15625Hz) 
	//Set timer to CTC mode
	//Max value of 16bit timer is 65535
	//Set OCR1A to 1s 
	//Enable interrupt on OCR1A
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	OCR1A = 15625;
	TIMSK1 |= (1 << OCIE1A);
}

void stop_game_waiting() {
	//Stop timer 1
	TCCR1B = 0;
	timer_count = 0;
}

int main() {
	DDRD |= (1 << LED_R) | (1 << LED_G) | (1 << LED_B);
	DDRB |= (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4);
	uart_init();
	i2c_init_as_slave();

	//Setup button SW1 as input
	//Detect falling edge on INT0
	//Enable interrupt on INT0
	DDRD &= ~(1 << PD2);
	EICRA |= (1 << ISC01) | (0 << ISC00);
	EIMSK |= (1 << INT0);

	sei();

	command_reset();

	for(;;);
}