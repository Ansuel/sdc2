#include <unistd.h>

#include "common.h"
#include "methods.h"

void send_thread_id_to_server(int server_fifo_desc, int thread_id) {
    char msg[10];
    sprintf(msg, "%d", thread_id);

    ssize_t bytes_to_send = strlen(msg) + 1;
    ssize_t bytes_sent = 0;
    while (bytes_sent < bytes_to_send) {
        ssize_t ret = write(server_fifo_desc, msg + bytes_sent, bytes_to_send - bytes_sent);
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Errore nella scrittura sulla server FIFO");
        bytes_sent += ret;
    }
}

ssize_t read_msg_from_server(int client_fifo_desc, char *buf, ssize_t max_len) {
    // read until a \0 is received, then return what has been received so far, \0 included
    ssize_t bytes_read = 0;
    while (1) {
        ssize_t ret = read(client_fifo_desc, buf + bytes_read, 1);
        if (ret == 0) return -1;
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Error nella lettura dalla client FIFO");
        bytes_read++;
        if (buf[bytes_read - 1] == '\0') break;
        if (bytes_read == max_len - 1) {
            buf[bytes_read - 1] = '\0';
            break;
        }
    }
    return bytes_read;
}
