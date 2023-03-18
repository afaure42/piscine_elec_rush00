#ifndef MASTER_COMMAND_H
# define MASTER_COMMAND_H

# include "slave_command.h"
# include "i2c.h"

uint8_t get_button_info(uint8_t slave_address);
uint8_t send_start(void); //no address since it will be a global call
uint8_t send_win(uint8_t slave_address);
uint8_t send_lose(uint8_t slave_address);
uint8_t send_reset(uint8_t slave_address);

#endif
