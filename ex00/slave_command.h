#ifndef SLAVE_COMMAND_H
# define SLAVE_COMMAND_H

# include <avr/io.h>

typedef enum e_command {
	COMMAND_BUTTON,
	COMMAND_START,
	COMMAND_WIN,
	COMMAND_LOSE,
	COMMAND_RESET
} command;

void exec_slave_command(command cmd);

#endif