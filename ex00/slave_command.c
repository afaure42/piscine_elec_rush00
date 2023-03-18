#include "slave_command.h"

void command_button() {
	uart_printstr("Received button command\r\n");
	if (current_slave_state == SLAVE_STATE_STARTING) {
		TWDR = button_pressed;
		uart_printstr("Sending status to master: ");
		uart_putnbr(button_pressed);
		uart_printstr("\r\n");
	} else if(current_slave_state == SLAVE_STATE_PLAYING) {
		TWDR = clicked_ms; //TODO Transform to 16 bits
		uart_printstr("Sending status to master: ");
		uart_putnbr(clicked_ms);
		uart_printstr("\r\n");
	}
}

void command_start() {
	uart_printstr("Received start command\r\n");
	PORTD &= ~(1 << LED_G);
	current_slave_state = SLAVE_STATE_PLAYING;
	start_game_waiting();
}

void command_win() {
	uart_printstr("Received win command\r\n");
	stop_game_waiting();
	PORTB &= ~(1 << LED1);
	PORTB &= ~(1 << LED2);
	PORTB &= ~(1 << LED3);
	PORTB &= ~(1 << LED4);
	PORTD |= (1 << LED_G);
}

void command_lose() {
	uart_printstr("Received lose command\r\n");
	stop_game_waiting();
	PORTB &= ~(1 << LED1);
	PORTB &= ~(1 << LED2);
	PORTB &= ~(1 << LED3);
	PORTB &= ~(1 << LED4);
	PORTD |= (1 << LED_R);
}

void command_reset() {
	uart_printstr("Received reset command\r\n");
	stop_game_waiting();
	timer_count = 0;
	clicked_ms = 0;
	button_pressed = 0;
	current_slave_state = SLAVE_STATE_STARTING;
	PORTB &= ~(1 << LED1);
	PORTB &= ~(1 << LED2);
	PORTB &= ~(1 << LED3);
	PORTB &= ~(1 << LED4);
	PORTD &= ~(1 << LED_R);
	PORTD &= ~(1 << LED_G);
	PORTD &= ~(1 << LED_B);
	PORTD |= (1 << LED_R);
}

void exec_slave_command(command cmd) {
	if (cmd == COMMAND_BUTTON) {
		command_button();
	} else if (cmd == COMMAND_START) {
		command_start();
	} else if (cmd == COMMAND_WIN) {
		command_win();
	} else if (cmd == COMMAND_LOSE) {
		command_lose();
	} else if (cmd == COMMAND_RESET) {
		command_reset();
	}
}