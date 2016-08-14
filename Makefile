CFLAGS=-O3 -W -Wall -std=c99 -g -lpigpio -lpthread

mhztest: mhztest.o

freq_count_1: freq_count_1.o

all: mhztest freq_count_1

clean:
	rm -f mhztest freq_count_1 *.o

