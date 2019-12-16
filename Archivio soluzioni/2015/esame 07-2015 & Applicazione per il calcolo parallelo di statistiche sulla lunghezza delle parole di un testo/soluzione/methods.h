#ifndef __METHODS_H__
#define __METHODS_H__

// file_io.c
char* load_from_file(const char *filename);
void save_to_file(const char *filename, const char *content);
void read_partial_count_from_file(int read_index, int *word_length, int *count);
void write_partial_count_to_file(int write_index, int word_length, int count);

// pipe_io.c
void write_word_to_pipe(int pipe_fd, const char *word);
int read_word_from_pipe(int pipe_fd, char *word);

// partial_count_buffer.c
void open_semaphores(void);
void read_partial_count(int *word_length, int *count);
void write_partial_count(int word_length, int count);
void close_semaphores(void);
void remove_semaphores(void);

#endif
