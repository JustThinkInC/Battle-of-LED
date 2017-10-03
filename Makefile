# File:   Makefile
# Author: M. P. Hayes, UCECE
# Date:   12 Sep 2010
# Descr:  Makefile for pseudo

# Definitions.
CC = avr-gcc
CFLAGS = -mmcu=atmega32u2 -Os -Wall -Wstrict-prototypes -Wextra -g -I. -I../drivers/avr -I../extra -I../utils -I../fonts -I../drivers
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm


# Default target.
all: pseudo.out


# Compile: create object files from C source files.
pseudo.o: pseudo.c ../drivers/avr/system.h ../drivers/avr/pio.h ../drivers/navswitch.h struct_init.h hashmap.h move.h collision.h
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

# Link: create output file (executable) from object files.
pseudo.out: pseudo.o system.o pio.o navswitch.o hashmap.o move.o collision.o  
	$(CC) $(CFLAGS) $^ -o $@ -lm
	$(SIZE) $@



# Target: clean project.
.PHONY: clean
clean: 
	-$(DEL) *.o *.out *.hex


# Target: program project.
.PHONY: program
program: pseudo.out
	$(OBJCOPY) -O ihex pseudo.out pseudo.hex
	dfu-programmer atmega32u2 erase; dfu-programmer atmega32u2 flash pseudo.hex; dfu-programmer atmega32u2 start


