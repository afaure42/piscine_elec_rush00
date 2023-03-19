#include "main.h"

volatile int ready = 0;
volatile int player_ready = 0;
volatile int timer_count = 0;
volatile game_state state = STARTING;
volatile int player_finished = 0;
volatile int winnable = 1;
volatile int result = 0;

void reset();

void timeout() {
	reset();
	i2c_send_byte(0x0, RESET_COMMAND);
}

ISR(TIMER1_COMPA_vect) {
	timer_count++;

	if (state == STARTING) {
		if (timer_count == TIMEOUT) {
			timeout();
		}
	} else if (state == PLAYING) {
		//Formula 1 LED
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
		if (timer_count == 2 * TIMER) {
			timeout();
		}
	}
}

void stop_timer() {
	TCCR1B = 0;
	TIMSK1 = 0;
}

void start_game() {
	uart_printstr("Check to start game\r\n");
	if (player_ready >= PLAYER_COUNT) {
		uart_printstr("Starting game\r\n");
		stop_timer();
		timer_count = 0;
		_delay_ms(500);
		PORTD &= ~(1 << LED_G);
		state = PLAYING;
	}
	//Timer configuration
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	OCR1A = 15625; // = 1s
	TIMSK1 |= (1 << OCIE1A);
}


void reset() {
	uart_printstr("Reset\r\n");

	stop_timer();

	state = STARTING;
	ready = 0;
	player_ready = 0;
	player_finished = 0;
	winnable = 1;
	timer_count = 0;
	result = 0;

	DDRD &= ~(1 << BUTTON);
	EICRA |= (1 << ISC01) | (0 << ISC00);
	EIMSK |= (1 << INT0);

	DDRD |= (1 << LED_R) | (1 << LED_G) | (1 << LED_B);
	DDRB |= (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4);

	PORTB &= ~(1 << LED1);
	PORTB &= ~(1 << LED2);
	PORTB &= ~(1 << LED3);
	PORTB &= ~(1 << LED4);
	PORTD &= ~(1 << LED_R);
	PORTD &= ~(1 << LED_G);
	PORTD &= ~(1 << LED_B);
	PORTD |= (1 << LED_R);
}

void blink(int led) {
	for(int i = 0; i < 5; i ++) {
		PORTD |= (1 << led);
		_delay_ms(200);
		PORTD &= ~(1 << led);
		_delay_ms(200);
	}
}

void check_end() {
	uart_printstr("Check to end game\r\n");
	if (player_finished >= PLAYER_COUNT) {
		uart_printstr("Ending game\r\n");
		PORTD &= ~(1 << LED_B);
		PORTD &= ~(1 << LED_R);
		if (result == 1) {
			uart_printstr("Win\r\n");
			blink(LED_G);
		} else {
			uart_printstr("Lose\r\n");
			blink(LED_R);
		}
		_delay_ms(1000);
		PORTD &= ~(1 << LED_R);
		PORTD &= ~(1 << LED_G);
		_delay_ms(500);
		reset();
	}
}

ISR(INT0_vect) {
	if (state == STARTING && !ready) {
		//Click to set ready
		uart_printstr("Ready\r\n");

		ready = 1;
		player_ready++;
		PORTD &= ~(1 << LED_R);
		PORTD |= (1 << LED_G);
		i2c_send_byte(0x0, READY_COMMAND);
		start_game();
		_delay_ms(20);
	} else if (state == PLAYING) {
		//Click in game
		state = CLICKED;
		uint32_t time = TCNT1 / 15.625 + (timer_count * 1000);

		uart_printstr("Clicked after ");
		uart_putnbr(time);
		uart_printstr("ms\r\n");

		stop_timer();
		PORTB &= ~(1 << LED1);
		PORTB &= ~(1 << LED2);
		PORTB &= ~(1 << LED3);
		PORTB &= ~(1 << LED4);
		player_finished++;
		if (time < TIMER * 1000) {
			//before
			uart_printstr("Too early\r\n");
			PORTD = (1 << LED_R);
			result = 0;
			winnable = 0;
			i2c_send_byte(0x0, LOSE_COMMAND);
		} else {
			//in time
			uart_printstr("In time ");
			if (winnable) {
				uart_printstr("and winnable\r\n");
			} else {
				uart_printstr("but not winnable\r\n");
			}
			PORTD = (1 << LED_B);
			result = winnable;
			i2c_send_byte(0x0, FINISH_COMMAND);
		}
		check_end();
	}
}

int main()
{
	uart_init();
	i2c_init();

	reset();
	sei();

	for(;;)
	{
		
	}
}
