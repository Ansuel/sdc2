#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "common.h"
#include "methods.h"

void log_message_helper(pid_t process_id, char* client_ip, uint16_t client_port, const char* msg, size_t msg_len) {
    char log_msg[2*MSG_SIZE];
    size_t log_msg_len;

    // preparazione prefisso
    sprintf(log_msg, "[PID %u - client %s:%hu] ", process_id, client_ip, client_port);
    log_msg_len = strlen(log_msg);

    // aggiunta suffisso
    memcpy(log_msg + log_msg_len, msg, msg_len);
    log_msg_len += msg_len;
    log_msg[log_msg_len++] = '\n';

    // scrittura messaggio
    log_message(log_msg, log_msg_len);
}
