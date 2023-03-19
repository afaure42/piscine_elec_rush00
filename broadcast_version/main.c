#include "main.h"

volatile int ready = 0;
volatile int player_ready = 0;
volatile int timer_count = 0;
volatile game_state state = STARTING;
volatile int player_finished = 0;
volatile int winnable = 1;
volatile int result = 0;
volatile error_state error = NOTHING;
volatile uint8_t reset_flag = 0;
volatile uint8_t do_check_end = 0;


void start_game();
void check_end();
void reset_game();
void timeout();
void stop_timer();

void trigger_resync()
{
	i2c_send_byte(0x0, RESET_COMMAND);
	reset_flag = 1;
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

ISR(TWI_vect)
{
	uart_printstr("Interrupt\r\n");
	uart_print_twi_status();
	if (TWSR == TW_SR_GCALL_DATA_NACK)
	{
		uart_printbyte(TWDR);
		uart_printstr("\r\n");
	}
	if (TWSR == TW_SR_GCALL_ACK)
	{
		uart_printstr("Received general call\r\n");

		uint8_t command = 0;
		i2c_receive_byte(&command, 1);
		uint8_t data = (command & 0xF0) >> 4;
		command = command & 0x0F;

		if (command == READY_COMMAND) {
			uart_printstr("Receive ready command\r\n");
			if (state != STARTING){
				uart_printstr("ERROR DESYNC\r\n");
				trigger_resync();
				reti();
				uart_printstr("THIS IS HORRIFIC\r\n");
			}
			player_ready++;
			if (player_ready!= data) {
				uart_printstr("ERROR DESYNC\r\n");
				trigger_resync();
				reti();
				uart_printstr("THIS IS HORRIFIC\r\n");
			}
			start_game();

		} else if (command == FINISH_COMMAND) {
			uart_printstr("Receive finish command\r\n");

			if (state != PLAYING && state != CLICKED) {
				uart_printstr("ERROR DESYNC\r\n");
				trigger_resync();
				reti();
				uart_printstr("THIS IS HORRIFIC\r\n");
			}
			player_finished++;
			if (player_finished != data) {
				uart_printstr("ERROR DESYNC\r\n");
				trigger_resync();
				reti();
				uart_printstr("THIS IS HORRIFIC\r\n");
			}
			winnable = 0;
			do_check_end = 1;

		} else if (command == LOSE_COMMAND) {
			uart_printstr("Receive lose command\r\n");

			if (state != PLAYING && state != CLICKED) {
				uart_printstr("ERROR DESYNC\r\n");
				trigger_resync();
				reti();
				uart_printstr("THIS IS HORRIFIC\r\n");
			}

			player_finished++;
			if (player_finished != data) {
				uart_printstr("ERROR DESYNC\r\n");
				trigger_resync();
				reti();
				uart_printstr("THIS IS HORRIFIC\r\n");
			}
			do_check_end = 1;

		} else if (command == RESET_COMMAND) {
			uart_printstr("Receive reset command\r\n");
			reset_flag = 1;
			// uart_printstr("help me god\r\n");
		}
	}
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
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
		// if (timer_count == 2 * TIMER) {
			// timeout();
		// }
	}
}

void timeout() {
	stop_timer();
	i2c_send_byte(0x0, RESET_COMMAND);
	uart_printstr("reset TIMEOUT\r\n");
	reset_game();
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
		_delay_ms(500);
		PORTD &= ~(1 << LED_G);
		state = PLAYING;
	}
	//Timer configuration
	timer_count = 0;
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	OCR1A = 15625; // = 1s
	TIMSK1 |= (1 << OCIE1A);
}


void reset_game() {
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
		stop_timer();
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
		if (reset_flag)
			reti();
		PORTD &= ~(1 << LED_R);
		PORTD &= ~(1 << LED_G);
		_delay_ms(500);
		uart_printstr("reset CHECK END\r\n");
		reset_flag = 1;
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
		if (i2c_send_byte(0x0, READY_COMMAND | ( player_ready << 4)) != 0)
		{
			reset_flag = 1;
			reti();
		}
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
			if(i2c_send_byte(0x0, LOSE_COMMAND | (player_finished << 4)) != 0)	{
				reset_flag = 1;
				reti();
			}
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
			if (i2c_send_byte(0x0, FINISH_COMMAND | (player_finished << 4)) != 0) {
				reset_flag = 1;
				reti();
			}
		}

		do_check_end = 1;
	}
}

int main()
{
	uart_init();
	i2c_init();

	reset_game();
	sei();

	for(;;)
	{
		if (reset_flag == 1)
		{
			reset_game();
			reset_flag = 0;
		}
		if (do_check_end == 1)
		{
			check_end();
			do_check_end = 0;
		}
	}
}
