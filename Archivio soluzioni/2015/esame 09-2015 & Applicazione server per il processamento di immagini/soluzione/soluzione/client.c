#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "common.h"

int read_file_to_buffer(char* buf, int src_fd, int len) {
    int ret;
    int bytes_read = 0;

    while (bytes_read < len) {
        ret = read(src_fd, buf + bytes_read, len - bytes_read);

        if (ret == 0) break; // no more bytes left to read
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Impossibile leggere dal file di input");

        bytes_read += ret;
    }

    return bytes_read;
}

void write_buffer_to_file(const char* buf, int dest_fd, int len) {
    int ret;
    int bytes_written = 0;

    while (bytes_written < len) {
        ret = write(dest_fd, buf + bytes_written, len - bytes_written);

        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Impossibile scrivere sul file di output");

        bytes_written += ret;
    }
}

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
        if (ret == 0) break; // endpoint closed connection
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Impossibile leggere da socket");

        bytes_rcvd += ret;
    }

    return bytes_rcvd;
}


void session(int sock_fd, int src_fd, int dest_fd) {
    int ret;
    int rcvd_bytes;

    char control_msg[CONTROL_MSG_LENGTH + 1];
    memset(control_msg, 0, CONTROL_MSG_LENGTH);

    // determine number of bytes to transfer
    struct stat file_stats;
    ret = fstat(src_fd, &file_stats);
    ERROR_HELPER(ret, "Impossibile determinare la dimensione del file di input");

    int input_file_bytes = file_stats.st_size;
    fprintf(stderr, "Numero di bytes da trasferire: %d\n", input_file_bytes);

    // read from src_fd into a buffer to simplify read operation
    char* read_buf = malloc(input_file_bytes);
    if (read_file_to_buffer(read_buf, src_fd, input_file_bytes) != input_file_bytes) {
        fprintf(stderr, "ERRORE: numero di bytes letti dal file di input non corretto\n");
        exit(1);
    }

    // send file length as initial message to server
    sprintf(control_msg, "%d", input_file_bytes);
    send_msg(sock_fd, control_msg, CONTROL_MSG_LENGTH);
    fprintf(stderr, "Lunghezza file input comunicata al server correttamente\n");

    // send whole file to server and free read buffer
    send_msg(sock_fd, read_buf, input_file_bytes);
    fprintf(stderr, "File inviato al server correttamente\n");
    free(read_buf);

    // receive output file length from server
    rcvd_bytes = recv_msg_of_given_length(sock_fd, control_msg, CONTROL_MSG_LENGTH);
    if (rcvd_bytes != CONTROL_MSG_LENGTH) {
        fprintf(stderr, "Ricevuti %d bytes invece che %d nel messaggio di risposta del server!\n", rcvd_bytes, CONTROL_MSG_LENGTH);
        exit(1);
    }

    // parse output file length and allocate write buffer
    control_msg[CONTROL_MSG_LENGTH] = '\0';
    int output_file_bytes = strtol(control_msg, NULL, 10);
    fprintf(stderr, "Dimensione del file di output: %d bytes\n", output_file_bytes);
    char* write_buf = malloc(output_file_bytes);

    // receive output file into write buffer
    rcvd_bytes = recv_msg_of_given_length(sock_fd, write_buf, output_file_bytes);
    if (rcvd_bytes != output_file_bytes) {
        fprintf(stderr, "Ricevuti %d bytes invece che %d per il file di output del server!\n", rcvd_bytes, output_file_bytes);
        exit(1);
    }

    // verify that there are no more bytes to read (i.e., endpoint has closed connection)
    ret = recv(sock_fd, control_msg, 1, 0); // we reuse control_msg buffer
    if (ret != 0) {
        fprintf(stderr, "ERRORE: numero di bytes inviati dal server per il file di output superiore a %d\n", output_file_bytes);
        exit(1);
    }

    // write buffer into dest_fd and free it
    write_buffer_to_file(write_buf, dest_fd, output_file_bytes);
    free(write_buf);

    fprintf(stderr, "File di output del server memorizzato correttamente!\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Sintassi: %s <port_number> <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int ret;

    // set IP address
    in_addr_t ip_addr = inet_addr("127.0.0.1");

    // retrieve port number
    long tmp = strtol(argv[1], NULL, 0);
    if (tmp < 1024 || tmp > 49151) {
        fprintf(stderr, "Utilizzare un numero di porta compreso tra 1024 e 49151.\n");
        exit(EXIT_FAILURE);
    }
    uint16_t port_number_no = htons((uint16_t)tmp); // we use network byte order

    int src_fd = open(argv[2], O_RDONLY);
    ERROR_HELPER(src_fd, "Impossibile aprire il file di input");

    // for simplicity we use rw-r--r-- permissions for the destination file
    int dest_fd = open(argv[3], O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (dest_fd < 0 && errno == EEXIST) {
        fprintf(stderr, "WARNING: file %s già presente, verrà sovrascritto!\n", argv[3]);
        dest_fd = open(argv[3], O_WRONLY | O_CREAT, 0644);
    }
    ERROR_HELPER(dest_fd, "Impossibile creare il file di destinazione");

    // all arguments have been processed: we can connect to the server
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_HELPER(socket_desc, "Impossibile creare una socket");

    // set up parameters for the connection
    struct sockaddr_in server_addr = {0}; // some fields are required to be filled with 0
    server_addr.sin_addr.s_addr = ip_addr;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = port_number_no;

    // initiate a connection on the socket
    ret = connect(socket_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
    ERROR_HELPER(ret, "Impossibile connettersi al server");

    fprintf(stderr, "Connessione al server stabilita!\n");

    // communicate with server
    session(socket_desc, src_fd, dest_fd);

    // close socket
    ret = close(socket_desc);
    ERROR_HELPER(ret, "Impossibile chiudere la socket");

    // close file descriptors
    ret = close(src_fd);
    ERROR_HELPER(ret, "Impossibile chiudere il file di input");

    ret = close(dest_fd);
    ERROR_HELPER(ret, "Impossibile chiudere il file di output");

    return 0;
}
