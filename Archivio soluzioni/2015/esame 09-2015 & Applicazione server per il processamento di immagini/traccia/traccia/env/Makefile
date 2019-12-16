SHELL = bash
CC = gcc -Wall -O0 -g
LDFLAGS = -lpthread

all: test env clean

test: test.c
	@$(CC) -o test test.c $(LDFLAGS)
	@./test

.PHONY: clean env test
clean:
	@rm -f test

env:
	@echo -e -n "Looking for gdb...\t"
	@which gdb
	@echo -e -n "Looking for evince...\t"
	@which evince


