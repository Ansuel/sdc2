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

    // open server FIFO
    server_fifo_desc = open(SERVER_FIFO, O_WRONLY);
    ERROR_HELPER(server_fifo_desc, "Errore nell'apertura della server FIFO");

    // launch reconstructor thread
    pthread_t reconstrutor_thread;
    ret = pthread_create(&reconstrutor_thread, NULL, reconstructor_thread_function, NULL);
    PTHREAD_ERROR_HELPER(ret, "Errore nel lancio del reconstructor thread");

    // launch reader threads
    pthread_t *reader_threads = (pthread_t*)malloc(reader_thread_count * sizeof(pthread_t));
    int i;
    for (i = 0; i < reader_thread_count; i++) {
        reader_thread_args_t* rt_args = (reader_thread_args_t*)malloc(sizeof(reader_thread_args_t));
        rt_args->thread_id = i;
        rt_args->server_fifo_desc = server_fifo_desc;
        ret = pthread_create(&reader_threads[i], NULL, reader_thread_function, rt_args);
        PTHREAD_ERROR_HELPER(ret, "Errore nel lancio del reader thread");
    }

    // wait for reader threads to terminate
    for (i = 0; i < reader_thread_count; i++) {
        ret = pthread_join(reader_threads[i], NULL);
        PTHREAD_ERROR_HELPER(ret, "Errore nella join del reader thread");
    }
    free(reader_threads);

    // put NULL msg into output buffer to make the reconstruction thread terminate
    put_char_msg(NULL);

    // communicate termination to server and close server FIFO
    send_thread_id_to_server(server_fifo_desc, CLIENT_TERMINATED);
    ret = close(server_fifo_desc);
    ERROR_HELPER(ret, "Errore nella chiusura della server FIFO");

    // wait for the reconstruction thread to terminate
    ret = pthread_join(reconstrutor_thread, NULL);
    PTHREAD_ERROR_HELPER(ret, "Errore nella join del reconstruction thread");

    destroy_char_msg_buffer();
    write_output_buffer_to_file();

    printf("Terminato\n");
    return EXIT_SUCCESS;
}
