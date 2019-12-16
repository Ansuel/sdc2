#ifndef UTIL_H
#define UTIL_H

#include <semaphore.h>

int send_data(int socket_desc, char * buf, int msg_len);
int receive_data(int socket_desc, char * buf);
int receive_data_len(int socket_desc, char * buf, int len);
sem_t * init_named_sem(char * name);
sem_t * open_named_sem(char * name);
int convert_to_int(char * buf, int len);
int read_file(int fd, char* buf, int len);
int read_file_char(int fd, char* buf, char c);
int write_file(int fd, char* buf, int len);

#endif