#ifndef COMMON_H
#define COMMON_H

// macros for handling errors
#define handle_error_en(en, msg)    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg)           do { perror(msg); exit(EXIT_FAILURE); } while (0)

/* Configuration parameters */
#define DEBUG           1   // display debug messages
#define MAX_CONN_QUEUE  3   // max number of connections the server can queue
#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_COMMAND  "BYE\n"
#define SERVER_PORT     2015

#ifdef DEBUG
#define debug_printf(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#else
#define debug_printf(...) do{ } while (0)
#endif

#endif
