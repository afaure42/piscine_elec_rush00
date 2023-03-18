#include "slave_command.h"

void command_button() {

}

void command_start() {

}

void command_win() {

}

void command_lose() {

}

void command_reset() {

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