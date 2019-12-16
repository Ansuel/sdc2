#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // htons() and inet_addr()
#include <netinet/in.h> // struct sockaddr_in
#include <sys/socket.h>
#include <fcntl.h>

#include "common.h"
#include "util.h"

int main(int argc, char* argv[]) {
    int ret = 0;
    char buf[1024];

    char* quit_command = SERVER_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    // variables for handling a socket
    int socket_desc;
    struct sockaddr_in server_addr = {0}; // some fields are required to be filled with 0

    // create a socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc < 0) handle_error("Could not create socket");

    // set up parameters for the connection
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(SERVER_PORT); // don't forget about network byte order!

    // initiate a connection on the socket
    ret = connect(socket_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
    if(ret) handle_error("Could not create connection");

    debug_printf("Connection established!\n");

    size_t buf_len = sizeof(buf);
    int msg_len = 0;
    memset(buf, 0, buf_len);

    // Here send initial data or receive data


    // main loop
    while (1) {
        
        // Do something here

        /* After a quit command we won't receive any more data from
         * the server, thus we must exit the main loop. */

        if (msg_len == quit_command_len && !memcmp(buf, quit_command, quit_command_len)) break;

    }


    // close the socket
    ret = close(socket_desc);
    if(ret) handle_error("Cannot close socket");

    debug_printf("Exiting...\n");

    exit(EXIT_SUCCESS);
}
