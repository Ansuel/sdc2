CC = gcc -Wall -O0 -g
LDFLAGS = -lpthread
ARCH = $(shell uname -m)

all: client server

client: client-$(ARCH)
	chmod +x client-$(ARCH)
	ln -s -f client-$(ARCH) client

server: common.h logger.c main.c send_recv.c util.c
	mkdir -p build
	$(CC) -c logger.c -o build/logger.o
	$(CC) -c main.c -o build/main.o
	$(CC) -c send_recv.c -o build/send_recv.o
	$(CC) -c util.c -o build/util.o
	$(CC) -o server build/*.o $(LDFLAGS)

:phony
clean:
	rm -f client server log.txt build/*.o
