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
    int ret;
    
		/**
		 *	INSERIRE QUI IL CODICE PER CREARE LA CLIENT FIFO
		 *	
		 *	Suggerimenti
		 *	- il nome è da usare è indicato dalla variabile client_fifo_name
		 *	- nel caso esista già (errno impostata a EEXIST), va rimossa e ricreata
		 *	- gestire eventuali errori		 		 
		 */

    // send thread id to server
    send_thread_id_to_server(rt_args->server_fifo_desc, rt_args->thread_id);

		/**
		 *	INSERIRE QUI IL CODICE PER L'APERTURA DELLA CLIENT FIFO IN SOLA LETTURA
		 *	
		 *	Suggerimenti
		 *	- gestire eventuali errori		 		 		 
		 *  - memorizzare il descrittore nella variabile client_fifo_desc
		 */
	int client_fifo_desc; 

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

		/**
		 *	INSERIRE QUI IL CODICE PER CHIUDERE E RIMUOVERE LA CLIENT FIFO
		 *	
		 *	Suggerimenti
		 *	- gestire eventuali errori
		 */		 		

    printf("[Reader %d] Terminato\n", rt_args->thread_id);
    free(rt_args);
    pthread_exit(NULL);
}
