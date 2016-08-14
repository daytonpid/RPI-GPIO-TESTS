CC=gcc
CFLAGS=-O3 -g
LDFLAGS=-lpigpio -lpthread
OBJECTS=freq_count_1.o

mhztest: mhztest.o

freq_count_1: $(OBJECTS)
        $(CC) $(CFLAGS) $(OBJECTS) -o freq_count_1 $(LDFLAGS)

all: mhztest freq_count_1

clean:
        rm -f mhztest freq_count_1 *.o
