#ifndef __METHODS_H__
#define __METHODS_H__

// threads.c
void setup_threads();

// socket.c
void send_msg(int sock_fd, const char *msg, int msg_len);
int recv_msg_of_given_length(int sock_fd, char* msg, int msg_len);

// util.c
unsigned char* parse_pgm (char* input_buf, int input_buf_len, int* p_width, int* p_height);
int pgm_to_ascii(unsigned char* input_img, int width, int height, char** p_output_buf);

#endif
