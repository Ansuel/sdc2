#include <pthread.h>
#include <fcntl.h>

#include "common.h"
#include "methods.h"

#define DEFAULT_READER_THREAD_COUNT 30

int reader_thread_count;

char output_buffer[MAX_OUTPUT_BUFFER_SIZE];
int max_output_buffer_index;

int server_fifo_desc;

void *reconstructor_thread_function(void *args) {
    max_output_buffer_index = 0;
    while (1) {
        char_msg_t *char_msg = get_char_msg();
        if (char_msg == NULL) break;
        output_buffer[char_msg->index] = char_msg->the_char;
        if (char_msg->index > max_output_buffer_index)
            max_output_buffer_index = char_msg->index;
        free(char_msg);
    }
    printf("[Reconstructor] Terminato, ricostruiti %d byte\n", (max_output_buffer_index + 1));
    pthread_exit(NULL);
}

void write_output_buffer_to_file(void) {

    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) ERROR_HELPER(-1, "Errore nell'apertura del file di output output.txt in modalità di sola scrittura");

    size_t bytes_to_write = max_output_buffer_index + 1;
    output_buffer[bytes_to_write] = '\0';
    size_t ret = fwrite(output_buffer, 1, bytes_to_write, output_file);
    if (ret == 0) ERROR_HELPER(-1, "Errore nella scrittura sul file di output output.txt");

    int close_ret = fclose(output_file);
    if (close_ret == EOF) ERROR_HELPER(-1, "Errore nella chiusura del file di output output.txt");
}

int main(int argc, char **argv) {

    int ret;
    if (argc == 1)
        reader_thread_count = DEFAULT_READER_THREAD_COUNT;
    else
        reader_thread_count = atoi(argv[1]);

    init_char_msg_buffer();

    /**
     *  INSERIRE QUI IL CODICE PER APRIRE LA SERVER FIFO IN SOLA SCRITTURA
     *
     *  Suggerimenti
     *  - il nome da usare è definito nella macro SERVER_FIFO
     *  - gestire eventuali errori
     *  - memorizzare il descrittore nella variabile globale
     *    preesistente server_fifo_desc
     */

        /**
         *  INSERIRE QUI IL CODICE PER LANCIARE IL RECONSTRUCTOR THREAD
         *
         *  Suggerimenti
         *  - la funzione che il reconstructor thread deve eseguire è reconstructor_thread_function(), che non richiede argomenti
         *  - sarà poi necessario attendere la fine del reconstructor thread
         *  - gestire eventuali errori
         */

    /**
     *  INSERIRE QUI IL CODICE PER LANCIARE I READER THREAD
     *
     *  Suggerimenti
     *  - il numero di reader thread da lanciare è indicato dalla variabile reader_thread_count
     *  - la funzione che ogni reader thread deve eseguire è reader_thread_function(), che richiede un argomento void*
     *    di cui farà un cast a reader_thread_args_t*
     *  - sarà poi necessario attendere la fine di ogni reader thread
     *  - gestire eventuali errori
     */

        /**
         *  INSERIRE QUI IL CODICE PER ATTENDERE LA FINE DEI READER THREAD
         *
         *  Suggerimenti
         *  - gestire eventuali errori
         *  - liberare risorse acquisite per poter lanciare i thread
         *  
         */

    // put NULL msg into output buffer to make the reconstruction thread terminate
    put_char_msg(NULL);

    // communicate termination to server and close server FIFO
    send_thread_id_to_server(server_fifo_desc, CLIENT_TERMINATED);

    /**
     *  INSERIRE QUI IL CODICE PER CHIUDERE LA SERVER FIFO
     *
     *  Suggerimenti
     *  - gestire eventuali errori
     */

    /**
         *  INSERIRE QUI IL CODICE PER ATTENDERE LA FINE DEL RECONSTRUCTOR THREAD
         *
         *  Suggerimenti
         *  - gestire eventuali errori
         */

    destroy_char_msg_buffer();
    write_output_buffer_to_file();

    printf("Terminato\n");
    return EXIT_SUCCESS;
}
