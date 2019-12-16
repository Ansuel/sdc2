#ifndef __METHODS_H__ // accorgimento per evitare inclusioni multiple di un header
#define __METHODS_H__

#include <stdlib.h>

// metodi definiti in logger.c
void initialize_logger(const char* logfile_name);
void log_message(const char* msg, size_t msg_len);

// metodi definiti in send_recv.c
size_t recv_msg(int socket_desc, char* buf, size_t max_len);
void send_msg(int socket, const char *msg, size_t msg_len);

// metodi definiti in util.c
void log_message_helper(pid_t process_id, char* client_ip, uint16_t client_port, const char* msg, size_t msg_len);


#endif
