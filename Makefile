# File:   Makefile
# Author: Theo Harbers & George Khella
# Date:   Oct 2017
# Descr:  Makefile for Game

# Definitions.
CC = avr-gcc
CFLAGS = -mmcu=atmega32u2 -Os -Wall -Wstrict-prototypes -Wextra -g -I. -I../drivers/avr -I../extra -I../utils -I../fonts -I../drivers
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm


# Default target.
all: game.out


# Compile: create object files from C source files.
game.o: game.c ../drivers/avr/system.h ../drivers/avr/pio.h ../drivers/navswitch.h struct_init.h hashmap.h move.h collision.h ../utils/tinygl.h ../utils/font.h ../utils/task.h ../drivers/avr/ir_uart.h ../drivers/avr/usart1.h ../extra/mmelody.h ../extra/ticker.h ../extra/tweeter.h sound.h
	$(CC) -c $(CFLAGS) $< -o $@

system.o: ../drivers/avr/system.c ../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

pio.o: ../drivers/avr/pio.c ../drivers/avr/pio.h ../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

navswitch.o: ../drivers/navswitch.c ../drivers/avr/delay.h ../drivers/avr/pio.h ../drivers/avr/system.h ../drivers/navswitch.h
	$(CC) -c $(CFLAGS) $< -o $@

hashmap.o: hashmap.c hashmap.h struct_init.h ../drivers/avr/pio.h
	$(CC) -c $(CFLAGS) $< -o $@

move.o: move.c move.h ../drivers/avr/pio.h struct_init.h
	$(CC) -c $(CFLAGS) $< -o $@

collision.o: collision.c collision.h hashmap.h move.h struct_init.h
	$(CC) -c $(CFLAGS) $< -o $@

timer.o: ../drivers/avr/timer.c ../drivers/avr/system.h ../drivers/avr/timer.h
	$(CC) -c $(CFLAGS) $< -o $@

ledmat.o: ../drivers/ledmat.c ../drivers/avr/pio.h ../drivers/avr/system.h ../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

display.o: ../drivers/display.c ../drivers/avr/system.h ../drivers/display.h ../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

font.o: ../utils/font.c ../drivers/avr/system.h ../utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@

tinygl.o: ../utils/tinygl.c ../drivers/avr/system.h ../drivers/display.h ../utils/font.h ../utils/tinygl.h
	$(CC) -c $(CFLAGS) $< -o $@

task.o: ../utils/task.c ../drivers/avr/system.h ../drivers/avr/timer.h ../utils/task.h
	$(CC) -c $(CFLAGS) $< -o $@

led.o: ../drivers/led.c ../drivers/avr/pio.h ../drivers/avr/system.h ../drivers/led.h
	$(CC) -c $(CFLAGS) $< -o $@
	
ir_uart.o: ../drivers/avr/ir_uart.c ../drivers/avr/ir_uart.h ../drivers/avr/pio.h ../drivers/avr/system.h ../drivers/avr/timer0.h ../drivers/avr/usart1.h
	$(CC) -c $(CFLAGS) $< -o $@

timer0.o: ../drivers/avr/timer0.c ../drivers/avr/bits.h ../drivers/avr/prescale.h ../drivers/avr/system.h ../drivers/avr/timer0.h
	$(CC) -c $(CFLAGS) $< -o $@

prescale.o: ../drivers/avr/prescale.c ../drivers/avr/prescale.h ../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

usart1.o: ../drivers/avr/usart1.c ../drivers/avr/system.h ../drivers/avr/usart1.h
	$(CC) -c $(CFLAGS) $< -o $@

mmelody.o: ../extra/mmelody.c ../drivers/avr/system.h ../extra/mmelody.h
	$(CC) -c $(CFLAGS) $< -o $@

ticker.o: ../extra/ticker.c
	$(CC) -c $(CFLAGS) $< -o $@

tweeter.o: ../extra/tweeter.c ../drivers/avr/system.h ../extra/ticker.h ../extra/tweeter.h
	$(CC) -c $(CFLAGS) $< -o $@

sound.o: sound.c sound.h ../extra/tweeter.h ../drivers/avr/pio.h ../extra/mmelody.h
	$(CC) -c $(CFLAGS) $< -o $@


# Link: create output file (executable) from object files.
game.out: game.o system.o pio.o navswitch.o hashmap.o move.o collision.o timer.o ledmat.o display.o font.o tinygl.o task.o led.o timer0.o ir_uart.o prescale.o usart1.o mmelody.o ticker.o tweeter.o sound.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
	$(SIZE) $@



# Target: clean project.
.PHONY: clean
clean: 
	-$(DEL) *.o *.out *.hex


# Target: program project.
.PHONY: program
program: game.out
	$(OBJCOPY) -O ihex game.out game.hex
	dfu-programmer atmega32u2 erase; dfu-programmer atmega32u2 flash game.hex; dfu-programmer atmega32u2 start


