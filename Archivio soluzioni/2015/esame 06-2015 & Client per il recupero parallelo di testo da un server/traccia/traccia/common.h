#ifndef __COMMON_H__ // accorgimento per evitare inclusioni multiple di un header
#define __COMMON_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define GENERIC_ERROR_HELPER(cond, errCode, msg) do {               \
        if (cond) {                                                 \
            fprintf(stderr, "%s: %s\n", msg, strerror(errCode));    \
            exit(EXIT_FAILURE);                                     \
        }                                                           \
    } while(0)

#define ERROR_HELPER(ret, msg)          GENERIC_ERROR_HELPER((ret < 0), errno, msg)
#define PTHREAD_ERROR_HELPER(ret, msg)  GENERIC_ERROR_HELPER((ret != 0), ret, msg)

// parametri di configurazione per il client
#define MAX_OUTPUT_BUFFER_SIZE (1 << 20) // 1MB
#define SERVER_FIFO "server_fifo"
#define CLIENT_TERMINATED -2

// dichiarazione di strutture dati in uso nel client
typedef struct char_msg_s {
    int index;
    char the_char;
} char_msg_t;

typedef struct reader_thread_args_s {
    int server_fifo_desc;
    int thread_id;
} reader_thread_args_t;

#endif
