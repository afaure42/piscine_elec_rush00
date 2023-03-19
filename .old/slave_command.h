#ifndef SLAVE_COMMAND_H
# define SLAVE_COMMAND_H

# include <avr/io.h>

# include "slave.h"

extern volatile int32_t timer_count;
extern volatile int32_t clicked_ms;
extern volatile int button_pressed;
extern volatile slave_state current_slave_state;

typedef enum e_command {
	COMMAND_BUTTON,
	COMMAND_START,
	COMMAND_WIN,
	COMMAND_LOSE,
	COMMAND_RESET
} command;

void exec_slave_command(command cmd);

#endif