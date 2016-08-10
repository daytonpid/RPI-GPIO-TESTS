CFLAGS=-O3 -W -Wall -std=c99 -D_XOPEN_SOURCE=500 -g

test: test.o mailbox.o

clean:
        rm -f test *.o

