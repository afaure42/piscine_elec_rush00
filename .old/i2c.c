#include "i2c.h"
#include <avr/interrupt.h>

const char hex_chars[] = "0123456789ABCDEF";

void i2c_start_write(uint8_t slave_address);
uint8_t i2c_write(unsigned char data);
void i2c_stop(void);
void i2c_start_read(uint8_t slave_address);
uint8_t i2c_read(uint8_t ack, uint8_t * buff);

ISR(TWI_vect)
{
	if (TW_STATUS == TW_ST_SLA_ACK)
	{
		// master is requesting data
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	if (TW_STATUS == TW_SR_DATA_ACK) {
		// master is sending data
		uint8_t data = TWDR;
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
}

uint8_t i2c_wait(void)
{
	while ((TWCR & (1 << TWINT)) == 0); //wait end of operation

	//TODO ADD A RETURN IN CASE OF TIMEOUT OR ERROR
	return 0;
}

void i2c_init(void)
{
	//setting SCL frequency in the bitrate register
	TWBR = TWI_BITRATE;

	//setting up own SLA addr and enabling general call
	TWAR = TWI_ADDR | 1 << TWGCE;
	//setting no prescaler 
	TWSR = 0;
}

void i2c_init_as_slave(void)
{
	i2c_init();
	TWAR = OWN_SLAVE_ADDRESS << 1;
	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
}

void i2c_send_full_command(uint8_t slave_address, uint8_t command, uint8_t param1, uint8_t param2)
{
	i2c_start_write(slave_address);
	i2c_write(command);
	i2c_write(param1);
	i2c_write(param2);
	i2c_stop();
}

void i2c_send_byte(uint8_t slave_address, uint8_t byte)
{
	uint8_t ret;
	i2c_start_write(slave_address);
	ret = i2c_write(byte);
	i2c_stop();
	return ret;
}

uint8_t i2c_read_byte(uint8_t slave_address, uint8_t * buffer, uint8_t size)
{
	i2c_start_read(slave_address);

	for(uint8_t i = 0; i < size; i++)
	{
		if (i2c_read(i == size - 1 ? NACK : ACK, buffer + i) == TIMEOUT)
			return TIMEOUT;
	}
	i2c_stop();
	return 0;
}

void i2c_start_write(uint8_t slave_address)
{
	//sending START on the TWI ( TWSTA for start, TWEN to enable TW, 
	//TWINT to clear theinterrupt flag and start operating)
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;

	while ((TWCR & (1 << TWINT)) == 0); //wait end of operation
	// uart_print_twi_status();

	//setting data register as temp sensor address and write mode
	TWDR = (slave_address << 1) | TW_WRITE;
	//sending SLA + W
	TWCR = 1 << TWINT | 1 << TWEN;

	while ((TWCR & (1 <<TWINT)) == 0); // wait for ACK
}

void i2c_start_read(uint8_t slave_address)
{

	//sending START on the TWI ( TWSTA for start, TWEN to enable TW, 
	//TWINT to clear theinterrupt flag and start operating)
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;

	while ((TWCR & (1 << TWINT)) == 0);

	TWDR = (slave_address << 1) | TW_READ;
	TWCR = 1 << TWINT | 1 << TWEN;

	while((TWCR & (1 << TWINT)) == 0);
}

void i2c_stop(void)
{
	//sending STOP on the TWI ( TWSTO for stop,
	// TWINT to clear interrupt flag and start operating)
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t i2c_write(unsigned char data)
{
	//moving data into data register
	TWDR = data;

	//sending data
	TWCR = 1 << TWINT | 1 << TWEN;

	return i2c_wait();
}

uint8_t i2c_read(uint8_t ack, uint8_t * buff)
{
	//setting TWI in receive mode
	TWCR = 1 << TWINT | 1 << TWEN | ack << TWEA;

	//waiting to receive data
	while((TWCR & (1 << TWINT)) == 0);
	//TODO detect timeout

	*buff = TWDR;
	return 0;
}

void uart_print_twi_status()
{
 	//extracting status code from TW status register
	uint8_t status = TWSR;
	status &= TW_STATUS_MASK;

	switch (status)
	{
	case TW_MASTER_START: {
		uart_printstr("A start condition has been transmitted\r\n");
		break;
	}
	case TW_MASTER_START_REPEAT: {
		uart_printstr("A repeated start condition has been transmitted\r\n");
		break;
	}
	case TW_MASTER_SLAW_ACK: {
		uart_printstr("SLA + W has been transmitted, ACK has been received\r\n");
		break;
	}
	case TW_MASTER_SLAW_NACK: {
		uart_printstr("SLA + W has been transmitted, NACK has been received\r\n");
		break;
	}
	case TW_MASTER_TRANSMIT_ACK: {
		uart_printstr("Data byte has been transmitted, ACK has been received\r\n");
		break;
	}
	case TW_MASTER_TRANSMIT_NACK: {
		uart_printstr("Data byte has been transmitted, NACK has been received\r\n");
		break;
	}
	case TW_MASTER_ARBITRATION_LOST: {
		uart_printstr("Arbitration lost\r\n");
		break;
	}
	case TW_MASTER_SLAR_ACK: {
		uart_printstr("SLA + R has been transmitted, ACK has been received\r\n");
		break;
	}
	case TW_MASTER_SLAR_NACK: {
		uart_printstr("SLA + R has been transmitted, NACK has been received\r\n");
		break;
	}
	case TW_MASTER_RECEIVE_ACK: {
		uart_printstr("Data byte has been received, ACK has been returned\r\n");
		break;
	}
	case TW_MASTER_RECEIVE_NACK: {
		uart_printstr("Data byte has been received, NACK has been returned\r\n");
		break;
	}
	default: {
		uart_printstr("Unkown status, status code is:");
		uart_printbyte(status);
		uart_printstr("\r\n");
		break;
	}
	}
}

char uart_rx(void)
{
	while (((UCSR0A >> RXC0) & 1) == 0);

	char ret = UDR0;
	return (ret);
}

void print_hex_value(unsigned char c)
{
	uint8_t to_print = c >> 4;
	uart_tx(hex_chars[to_print]);
	to_print = c & 0b1111;
	uart_tx(hex_chars[to_print]);
}
