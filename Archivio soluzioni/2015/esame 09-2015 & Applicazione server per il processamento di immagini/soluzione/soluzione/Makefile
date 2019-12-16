CC = gcc -Wall -O0 -g
LDFLAGS = -lpthread

all: client server

client: common.h client.c
	$(CC) -o client client.c

server: common.h methods.h server.c socket.c threads.c util.c
	mkdir -p build
	$(CC) -c server.c -o build/server.o
	$(CC) -c socket.c -o build/socket.o
	$(CC) -c threads.c -o build/threads.o
	$(CC) -c util.c -o build/util.o
	$(CC) -o server build/*.o $(LDFLAGS)

:phony
clean:
	rm -rf client server build
