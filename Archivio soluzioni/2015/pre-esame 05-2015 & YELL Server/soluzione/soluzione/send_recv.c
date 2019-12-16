#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "common.h"
#include "methods.h"

size_t recv_msg(int socket_desc, char* buf, size_t max_len) {
    size_t msg_len;
    while ( (msg_len = recv(socket_desc, buf, max_len, 0)) <= 0 ) {
        if (msg_len == 0) return -1; // endpoint closed connection
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot read from socket");
    }
    return msg_len;
}

void send_msg(int socket, const char *msg, size_t msg_len) {
    int ret;
    int bytes_sent = 0;

    while (bytes_sent < msg_len) {
        ret = send(socket, msg + bytes_sent, msg_len - bytes_sent, 0);

        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Cannot write to socket");

        bytes_sent += ret;
    }
}
