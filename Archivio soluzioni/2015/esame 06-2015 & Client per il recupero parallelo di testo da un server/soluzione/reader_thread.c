#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"
#include "methods.h"

int msg_to_char_msg(char *msg, char_msg_t *char_msg) {
    size_t msg_len = strlen(msg);
    char tmp_index[10];
    strncpy(tmp_index, msg, msg_len - 2);
    tmp_index[msg_len-2] = '\0'; // required by atoi to work properly
    char_msg->index = atoi(tmp_index);
    char_msg->the_char = msg[msg_len - 1];
    return 0;
}

void *reader_thread_function(void *args) {

    reader_thread_args_t* rt_args = (reader_thread_args_t*)args;

    // create client FIFO
    char client_fifo_name[10];
    sprintf(client_fifo_name, "%d", rt_args->thread_id);
    int ret = mkfifo(client_fifo_name, 0666);
    if (ret == -1 && errno == EEXIST) {
        ret = unlink(client_fifo_name);
        ERROR_HELPER(ret, "Errore nella rimozione della client FIFO");
        ret = mkfifo(client_fifo_name, 0666);
        /* Il controllo sugli errori viene fatto subito in uscita dal blocco,
         * in modo da accorparlo con il check previsto per la prima mkfifo()
         * per errori diversi da EEXIST. */
        //ERROR_HELPER(ret, "Errore nella creazione della client FIFO");
    }
    ERROR_HELPER(ret, "Errore nella creazione della client FIFO");

    // send thread id to server
    send_thread_id_to_server(rt_args->server_fifo_desc, rt_args->thread_id);

    // and open it in read-only mode
    int client_fifo_desc = open(client_fifo_name, O_RDONLY);
    ERROR_HELPER(client_fifo_desc, "Errore nell'apertura della client FIFO in modalità di sola lettura");

    // keep reading msg from client FIFO until an end msg is read
    char msg[10];
    while (1) {
        ssize_t ret = read_msg_from_server(client_fifo_desc, msg, 10);
        if (ret == -1) break; // Server closed its client FIFO descriptor
        char_msg_t *char_msg = (char_msg_t*)malloc(sizeof(char_msg_t));
        ret = msg_to_char_msg(msg, char_msg);
        if (ret) {
            fprintf(stderr, "Errore nella conversione del messaggio \"%s\" nella struttura char_msg_t\n", msg);
            exit(EXIT_FAILURE);
        }
        if (char_msg->index == -1) {
            free(char_msg);
            break; // end msg received
        }
        put_char_msg(char_msg); // performs free(char_msg) too
    }

    // close and unlink the FIFO
    ret = close(client_fifo_desc);
    ERROR_HELPER(ret, "Errore nella chiusura della client FIFO");
    ret = unlink(client_fifo_name);
    ERROR_HELPER(ret, "Errore nella rimozione della client FIFO");

    printf("[Reader %d] Terminato\n", rt_args->thread_id);
    free(rt_args);
    pthread_exit(NULL);
}
