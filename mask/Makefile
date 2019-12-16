CC = gcc -Wall -g

all: client server

client: client.c common.h
	$(CC) -o client client.c util.c -lpthread

server: server.c common.h
	$(CC) -o server server.c util.c -lpthread

.PHONY: clean

clean:
	rm -f client server
