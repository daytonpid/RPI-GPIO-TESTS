CC=gcc
CFLAGS=-O3 -g
LDFLAGS=-lpigpio -lpthread

busywait_test: busywait_test.o
	$(CC) $(CFLAGS) busywait_test.o -o busywait_test

pigpio_pulse: pigpio_pulse.o
	$(CC) $(CFLAGS) pigpio_pulse.o -o pigpio_pulse $(LDFLAGS)

all: busywait_test pigpio_pulse

clean:
	rm -f busywait_test pigpio_pulse *.o
