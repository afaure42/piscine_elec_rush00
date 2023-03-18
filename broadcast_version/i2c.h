#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include "uart.h"

#define SCL_FREQUENCY 100000
#define TWI_BITRATE (((F_CPU / SCL_FREQUENCY) - 16) / 2)
#define TW_WRITE 0
#define TW_READ 1
#define TW_ACK 0
#define TW_NACK 1
#define TW_MASTER_START 0x08
#define TW_MASTER_START_REPEAT 0x10
#define TW_MASTER_SLAW_ACK 0x18
#define TW_MASTER_SLAW_NACK 0x20
#define TW_MASTER_TRANSMIT_ACK 0x28
#define TW_MASTER_TRANSMIT_NACK 0x30
#define TW_MASTER_ARBITRATION_LOST 0x38
#define TW_MASTER_SLAR_ACK 0x40
#define TW_MASTER_SLAR_NACK 0x48
#define TW_MASTER_RECEIVE_ACK 0x50
#define TW_MASTER_RECEIVE_NACK 0x58


#define ACK 1
#define NACK 0

#define TWI_ADDR 0x10

#define TIMEOUT 1

#define READY_COMMAND 0x01
#define FINISH_COMMAND 0x02
#define LOSE_COMMAND 0x03

void i2c_send_byte(uint8_t slave_address, uint8_t byte);

/**
 * @brief 
 *  will read bytes sending ACK until buffer is full sending NACK at the last byte
 * @param slave_address 
 * @param buffer 
 * @param size
 * @return uint8_t 
 */
uint8_t i2c_read_byte(uint8_t slave_address, uint8_t * buffer, uint8_t buffer_size);
void i2c_send_full_command(uint8_t slave_address, uint8_t command, uint8_t param1, uint8_t param2);
void i2c_init(void);
void i2c_init_as_slave(void);

#endif
