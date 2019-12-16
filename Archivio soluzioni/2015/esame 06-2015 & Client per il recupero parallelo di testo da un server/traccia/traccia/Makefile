CC = gcc -Wall -O0 -g
LDFLAGS = -lpthread
ARCH = $(shell uname -m)

all: client server

client: common.h methods.h char_msg_buffer.c read_write.c reader_thread.c client.c
	mkdir -p build
	$(CC) -c char_msg_buffer.c -o build/char_msg_buffer.o
	$(CC) -c read_write.c -o build/read_write.o
	$(CC) -c reader_thread.c -o build/reader_thread.o
	$(CC) -c client.c -o build/client.o
	$(CC) -o client build/*.o $(LDFLAGS)

server: server-$(ARCH)
	ln -s -f server-$(ARCH) server

:phony
clean:
	rm -f client server build/*.o
