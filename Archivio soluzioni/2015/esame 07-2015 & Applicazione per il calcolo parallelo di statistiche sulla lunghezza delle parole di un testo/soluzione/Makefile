CC = gcc -Wall -O0 -g
LDFLAGS = -lpthread

all: word_length_counter

word_length_counter: common.h methods.h file_io.c pipe_io.c partial_count_buffer.c word_length_counter.c
	mkdir -p build
	$(CC) -c file_io.c -o build/file_io.o
	$(CC) -c pipe_io.c -o build/pipe_io.o
	$(CC) -c partial_count_buffer.c -o build/partial_count_buffer.o
	$(CC) -c word_length_counter.c -o build/word_length_counter.o
	$(CC) -o word_length_counter build/*.o $(LDFLAGS)

:phony
clean:
	rm -f word_length_counter build/*.o
