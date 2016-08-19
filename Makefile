CC=gcc
CFLAGS=-O3
LDFLAGS=-lpigpio -lpthread

busywait_test: busywait_test.o
	$(CC) $(CFLAGS) busywait_test.o -o busywait_test $(LDFLAGS)

led_test: led_test.o
	$(CC) $(CFLAGS) led_test.o -o led_test

all: busywait_test led_test

clean:
	rm -f busywait_test led_test *.o
