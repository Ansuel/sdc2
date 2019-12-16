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

// parametri di configurazione per il server
#define MAX_CONCURRENCY     4
#define MAX_CONN_QUEUE      16
#define LOGFILE_NAME        "log.txt"

#define MSG_SIZE            1024
#define BYE_COMMAND         "BYE"

#endif
