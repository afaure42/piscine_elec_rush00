#ifndef SLAVE_H
# define SLAVE_H

# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>
# include <util/twi.h>

# include "uart.h"
# include "i2c.h"

# define LED_R PD5
# define LED_G PD6
# define LED_B PD3

# define LED1 PB4
# define LED2 PB2
# define LED3 PB1
# define LED4 PB0

# define TIMER 5

typedef enum e_slave_state {
	SLAVE_STATE_STARTING,
	SLAVE_STATE_PLAYING
} slave_state;

void start_game_waiting(void);
void stop_game_waiting();

#endif