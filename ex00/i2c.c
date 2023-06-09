#include "i2c.h"
#include "uart.h"

const char hex_chars[] = "0123456789ABCDEF";

void i2c_start_write(uint8_t slave_address);
void i2c_write(unsigned char data);
void i2c_stop(void);
void i2c_start_read(uint8_t slave_address);
uint8_t i2c_read(uint8_t ack, uint8_t * buff);

void i2c_init(void)
{
	//setting SCL frequency in the bitrate register
	TWBR = TWI_BITRATE;

	//setting up own SLA addr and enabling general call
	TWAR |= (1<<TWGCE);
	//setting no prescaler 
	TWSR = 0;

	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWEN);
}

uint8_t i2c_wait(void)
{
	while ((TWCR & (1 << TWINT)) == 0); //wait end of operation

	//TODO ADD A RETURN IN CASE OF TIMEOUT OR ERROR
	return 0;
}

void i2c_send_full_command(uint8_t slave_address, uint8_t command, uint8_t param1, uint8_t param2)
{
	i2c_wait();
	i2c_start_write(slave_address);
	i2c_write(command);
	i2c_write(param1);
	i2c_write(param2);
	i2c_stop();
}

uint8_t i2c_receive_byte(uint8_t * buffer, uint8_t size)
{
	uart_printstr("Receiving bytes in twi\r\n");
	for(uint8_t i = 0; i < size; i++)
	{
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
		if (i2c_wait() != 0)
			return 1;
		uart_print_twi_status();
		*buffer = TWDR;
	}
	TWCR = (1 << TWINT) | (1 << TWEN);// | (1 << TWEA);
	i2c_wait();
	uart_print_twi_status();
	return 0;
}

uint8_t i2c_send_byte(uint8_t slave_address, uint8_t byte)
{
	uart_printstr("waiting before send\r\n");
	i2c_start_write(slave_address);
	if (TWSR != TW_MT_SLA_ACK)	{
		i2c_stop();
		return (1);
	}
	uart_print_twi_status();
	i2c_write(byte);
	if (TWSR != TW_MT_DATA_ACK) {
		i2c_stop();
		return (1);
	}
	uart_print_twi_status();
	i2c_stop();
	uart_printstr("byte sent\r\n");
	return (0);
}

uint8_t i2c_read_byte(uint8_t slave_address, uint8_t * buffer, uint8_t size)
{
	i2c_wait();
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
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN | 1 << TWIE;

	while ((TWCR & (1 << TWINT)) == 0); //wait end of operation
	// uart_print_twi_status();


	//setting data register as temp sensor address and write mode
	TWDR = (slave_address << 1) | TW_WRITE;
	//sending SLA + W
	TWCR = (1 << TWINT) | (1 << TWIE) | (1 << TWEA) | (1 << TWEN);

	while ((TWCR & (1 <<TWINT)) == 0); // wait for ACK
}

void i2c_start_read(uint8_t slave_address)
{

	//sending START on the TWI ( TWSTA for start, TWEN to enable TW, 
	//TWINT to clear theinterrupt flag and start operating)
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;

	while ((TWCR & (1 << TWINT)) == 0);

	TWDR = (slave_address << 1) | TW_READ;
	TWCR |= 1 << TWINT | 1 << TWEN;

	while((TWCR & (1 << TWINT)) == 0);
}

void i2c_stop(void)
{
	//sending STOP on the TWI ( TWSTO for stop,
	// TWINT to clear interrupt flag and start operating)
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEA) | (1 << TWEN)
			| (1 << TWIE);
}

void i2c_write(unsigned char data)
{
	//moving data into data register
	TWDR = data;

	//sending data
	TWCR |= 1 << TWINT;

	while ((TWCR & (1 << TWINT)) == 0); // wait until it is done
}

uint8_t i2c_read(uint8_t ack, uint8_t * buff)
{
	//setting TWI in receive mode
	TWCR |= 1 << TWINT | 1 << TWEN | ack << TWEA;

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
	uart_printstr("\t\t");
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
	case TW_MT_DATA_ACK: {
		uart_printstr("Data byte has been transmitted, ACK has been received\r\n");
		break;
	}
	case TW_MT_DATA_NACK: {
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
	case TW_SR_SLA_ACK: {
		uart_printstr("Slave receiver received SLA + W, ACK returned\r\n");
		break;
	}
	case TW_SR_GCALL_ACK: {
		uart_printstr("Slave receiver received GCALL, ACK returned\r\n");
		break;
	}
	case TW_SR_GCALL_DATA_ACK: {
		uart_printstr("Slave receiver received GCALL DATA, ACK returned\r\n");
		break;
	}
	case TW_SR_GCALL_DATA_NACK: {
		uart_printstr("Slave receiver received GCALL DATA, NACK returned\r\n");
		break;
	}
	case TW_SR_DATA_ACK: {
		uart_printstr("Slave receiver received data, ACK returned\r\n");
		break;
	}
	case TW_SR_DATA_NACK: {
		uart_printstr("Slave receiver received data, NACK returned\r\n");
		break;
	}
	case TW_SR_STOP: {
		uart_printstr("Slave receiver received STOP\r\n");
		break;
	}
	case TW_BUS_ERROR: {
		uart_printstr("TWI BUS ERROR\r\n");
		break;
	}
	case TW_NO_INFO: {
		uart_printstr("TWI NO INFO\r\n");
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
