DEVICE=atmega328p
F_CPU = 16000000UL
BAUDRATE=115200
PROGRAMMER= -c arduino -P /dev/ttyUSB0 -b$(BAUDRATE) -p $(DEVICE)

SRCS = slave.c uart.c i2c.c slave_command.c

BINS = $(SRCS:.c=.bin)
HEX = main.hex

CC = avr-gcc


CFLAGS = -Wall -D F_CPU=$(F_CPU) -D UART_BAUDRATE=$(BAUDRATE) -mmcu=$(DEVICE) -Os

all: flash



%.bin: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(HEX): $(BINS)
	$(CC) $(CFLAGS) -o linked.bin ${BINS}
	avr-objcopy -O ihex linked.bin ${HEX}



hex: $(HEX)

flash: hex
	avrdude $(PROGRAMMER) -U flash:w:$(HEX):i

clean:
	rm -rf $(BINS) $(HEX)
	
re: clean all