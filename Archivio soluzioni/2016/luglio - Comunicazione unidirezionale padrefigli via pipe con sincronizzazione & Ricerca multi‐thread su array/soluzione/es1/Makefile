CC = gcc -Wall -O0 -g
LDFLAGS = -lpthread

all: main

main: common.h main.c
	rm -f build/*.o
	$(CC) -c main.c -o build/main.o
	$(CC) -o main build/*.o $(LDFLAGS)

:phony
clean:
	rm -f main build/*.o
