#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include "i2c.h"

#define I2C_ADDR 0xF8

#define GAME_LOST	0xFF
#define GAM                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   E_WON	0x00

#define TWI_SLAVE 1
#define TWI_MASTER 0

#define SLAVE_NUMBER 1

#define SLAVE_ADDRESS1 0x40

#define START_PHASE 1
#define COOLDOWN_PHASE 2
#define FIRE_PHASE 3
#define END_PHASE 4
#define SYNC_PHASE 0

#define START_COOLDOWN_COMMAND 0x1

uint8_t g_twi_status = 0;
uint8_t	sw1_status = 0;
uint8_t game_status = 0;


void master_launch_game_phase()
{
	uint8_t slave1_status;
	//wait for own button to be pressed

	//keep polling slaves until they are ready
	while (slave1_status == 0)
	{
		//get button status
		//if timeout or error go back to sync phase
		{
			game_status = SYNC_PHASE;
			return;
		}
	}

	game_status = COOLDOWN_PHASE;
}

void master_launch_cooldown_phase()
{

	//send cooldown start order as a general call

	if (TWSR == TW_MASTER_TRANSMIT_NACK)
	{
		//idk
	}
	//start own cooldown
}

void master_fire_phase()
{
	//keep polling slaves until everyone fired or max game length reached
	
	//if someone fires before end of cooldown, send lose

	//if i fire before end of cooldown i lose
}

void master_wait_and_reset()
{
	//let winner celebrate and losers weep a bit

	//send reset signal to every slave
}

int main()
{
	uart_init();
	i2c_init();

	while (1)
	{
		//first phase is determining master if it isnt determined

		//second phase is waiting for everyone to light buttons

		//third phase is cooldown

		//fourth phase is FIRE

	}
}
