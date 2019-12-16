#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

#define END_CHAR_MSG '\n'

int read_file_char(int fd, char* buf, char c) {
    int ret, bytes_read = 0;

    while(buf[bytes_read] != c) {
        ret = read(fd, buf+bytes_read, 1);
        // Aggiungere handle_error_en(-1, "Chiusura inattesa del fd");
        // Se si è in una fifo AL POSTO DI BREAK
        if (ret == 0) break;
        if (ret == -1 ) {
            if (errno == EINTR) continue;
            handle_error("Error read");
        }
        bytes_read += ret;
    }

    return bytes_read;
}

int read_file(int fd, char* buf, int len) {
    int ret, bytes_read = 0;

    while(bytes_read < len) {
        ret = read(fd, buf+bytes_read, len-bytes_read);
        // Aggiungere handle_error_en(-1, "Chiusura inattesa del fd");
        // Se si è in una fifo AL POSTO DI BREAK
        if (ret == 0) break;
        if (ret == -1 ) {
            if (errno == EINTR) continue;
            handle_error("Error read");
        }
        bytes_read += ret;
    } 

    return bytes_read;
}

int write_file(int fd, char* buf, int len) {
    int ret, written_bytes = 0;

    while(written_bytes < len) {
        ret = write(fd, buf+written_bytes, len-written_bytes);
        if (ret == -1 ) {
            if (errno == EINTR) continue;
            handle_error("Error write");
        }
        written_bytes += ret;
    } 

    return written_bytes;
}

void IS_ERR(int ret, char * msg) {
    if (ret == -1) handle_error(msg);

}

// Inizialize semaphore
sem_t * init_named_sem(char * name) {
    sem_unlink(name);

    sem_t * sem = sem_open(name, O_CREAT | O_EXCL, 0666, 0);
    if (sem == SEM_FAILED) handle_error("Error open sem");

    return sem;
}

// Open already created semaphore
sem_t * open_named_sem(char * name) {
    sem_t * sem = sem_open(name, 0);
    if (sem == SEM_FAILED) handle_error("Error open sem");

    return sem;
}

int convert_to_int(char * buf, int len) {
    int val = 0;

    char tmp[len];
    memcpy(tmp,buf,len);
    val = atoi(tmp);

    return val;
}

int send_data(int socket_desc, char * buf, int msg_len) {

    int bytes_sent = 0,  ret;

    do {
        ret = send(socket_desc, buf+bytes_sent, msg_len-bytes_sent, 0);
        if (ret == -1) {
            if (errno == EINTR) continue;
            handle_error("Errore send");
        }
        bytes_sent += ret;
    } while (bytes_sent != msg_len);

    debug_printf("Sent: %d\n", bytes_sent);
    return bytes_sent;
}

int receive_data(int socket_desc, char * buf) {
    int bytes_recv = 0, ret;

    do {
        ret = recv(socket_desc, buf+bytes_recv, 1, 0);
        if (ret == -1) {
            if (errno == EINTR) continue;
            handle_error("Errore send");
        }
        if (ret == 0) break;
        bytes_recv += ret;
    } while(buf[bytes_recv-1] != END_CHAR_MSG);

    debug_printf("Received: %d\n", bytes_recv);

    return bytes_recv;
}

int receive_data_len(int socket_desc, char * buf, int len) {
    int bytes_recv = 0, ret;

    do {
        ret = recv(socket_desc, buf+bytes_recv, 1, 0);
        if (ret == -1) {
            if (errno == EINTR) continue;
            handle_error("Errore send");
        }
        if (ret == 0) break;
        bytes_recv += ret;
    } while(bytes_recv < len);



    debug_printf("Ricevuti: %d\n", bytes_recv);
    return bytes_recv;
}