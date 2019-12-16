#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <unistd.h>

#include "common.h"

// char_msg_buffer.c
void init_char_msg_buffer();
char_msg_t *get_char_msg();
void put_char_msg(char_msg_t *char_msg);
void destroy_char_msg_buffer();

// read_write.c
void send_thread_id_to_server(int server_fifo_desc, int thread_id);
ssize_t read_msg_from_server(int client_fifo_desc, char *buf, ssize_t max_len);

// reader_thread.c
void *reader_thread_function(void *args);

#endif
