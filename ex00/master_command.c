#include "master_command.h"

uint8_t get_button_timing(uint8_t slave_address, uint16_t * time_buffer)
{
	if (i2c_send_byte(slave_address, COMMAND_BUTTON) == TIMEOUT)
		return TIMEOUT;
	if (i2c_read_byte(slave_address, time_buffer, 2) == TIMEOUT)
		return TIMEOUT;
	return 0;
}

uint8_t get_button(uint8_t slave_address, uint8_t * button_buffer)
{
	if (i2c_send_byte(slave_address, COMMAND_BUTTON) == TIMEOUT)
		return TIMEOUT;
	if (i2c_read_byte(slave_address, button_buffer, 1) == TIMEOUT)
		return TIMEOUT;
	return 0;
}

uint8_t send_start(void)
{
	return i2c_send_byte(TW_GLOBAL, COMMAND_START);
}

uint8_t send_reset(void)
{
	return i2c_send_byte(TW_GLOBAL, COMMAND_RESET);
}

uint8_t send_lose(uint8_t slave_address)
{
	if (i2c_send_byte(slave_address, COMMAND_LOSE) == TIMEOUT)
		return TIMEOUT;
	return 0;
}
uint8_t send_win(uint8_t slave_address)
{
	if (i2c_send_byte(slave_address, COMMAND_WIN) == TIMEOUT)
		return TIMEOUT;
	return 0;
}
