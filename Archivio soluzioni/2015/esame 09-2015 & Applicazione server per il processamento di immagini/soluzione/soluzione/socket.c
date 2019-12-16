#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

void send_msg(int sock_fd, const char *msg, int msg_len) {
    int ret;
    int bytes_sent = 0;

    while (bytes_sent < msg_len) {
        ret = send(sock_fd, msg + bytes_sent, msg_len - bytes_sent, 0);

        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Impossibile scrivere su socket");

        bytes_sent += ret;
    }
}

int recv_msg_of_given_length(int sock_fd, char* msg, int msg_len) {
    int ret;

    int bytes_rcvd = 0;
    while (bytes_rcvd < msg_len) {
        ret = recv(sock_fd, msg + bytes_rcvd, msg_len - bytes_rcvd, 0);
        if (ret == 0) return 0; // endpoint closed connection
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Impossibile leggere da socket");

        bytes_rcvd += ret;
    }

    return bytes_rcvd;
}
