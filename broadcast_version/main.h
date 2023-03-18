#ifndef MAIN_H
# define MAIN_H

# include <avr/io.h>
# include <util/delay.h>
# include <util/twi.h>
# include "i2c.h"
# include "uart.h"

# define LED_R PD5
# define LED_G PD6
# define LED_B PD3

# define LED1 PB4
# define LED2 PB2
# define LED3 PB1
# define LED4 PB0

# define BUTTON PD2

# define TIMER 5

# define PLAYER_COUNT 1

typedef enum e_game_state {
	STARTING,
	PLAYING,
	CLICKED,
} game_state;

#endif