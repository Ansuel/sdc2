#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"
#include "methods.h"

// variabili globali definite in altri moduli
extern sem_t semaphores[NUM_THREADS];

// variabili globali definite nel modulo corrente
unsigned char* input_img;
volatile unsigned char* output_img;
int width, height;

void session(int sock_fd) {
    char control_msg[CONTROL_MSG_LENGTH + 1];

    int rcvd_bytes = recv_msg_of_given_length(sock_fd, control_msg, CONTROL_MSG_LENGTH);
    if (rcvd_bytes != CONTROL_MSG_LENGTH) {
        fprintf(stderr, "Ricevuti %d bytes invece che %d nel messaggio iniziale del client!\n", rcvd_bytes, CONTROL_MSG_LENGTH);
        exit(1);
    }

    control_msg[CONTROL_MSG_LENGTH] = '\0';
    int input_file_bytes = strtol(control_msg, NULL, 10);
    fprintf(stderr, "Dimensione del file di input: %d bytes\n", input_file_bytes);

    char* read_buf = malloc(input_file_bytes + 1); // byte extra per ottimizzazione del codice in util.c
    rcvd_bytes = recv_msg_of_given_length(sock_fd, read_buf, input_file_bytes);
    if (rcvd_bytes != input_file_bytes) {
        fprintf(stderr, "Ricevuti %d bytes invece che %d per il file di input del client!\n", rcvd_bytes, input_file_bytes);
        exit(1);
    }
    read_buf[input_file_bytes] = '\n';

    input_img = parse_pgm(read_buf, input_file_bytes + 1, &width, &height);
    free(read_buf);

    output_img = malloc(width*height);

    /** [PUNTO 3] INIZIO DELLA SESSIONE DI PROCESSAMENTO
     *
     * Obiettivo:
     * Il main dovrà notificare a ciascun thread la disponibilità di una
     * nuova immagine da processare incrementando per ogni thread il
     * semaforo ad esso associato. Dopo il messaggio di stampa, il main
     * dovrà attendere che ogni thread abbia incrementato il semaforo
     * associato prima di proseguire con le istruzioni successive.
     *
     * Note: gestire eventuali errori sui semafori
     *
     **/

    fprintf(stderr, "Processamento dell'immagine in corso...\n");

    /** [PUNTO 3] FINE DELLA SESSIONE DI PROCESSAMENTO **/

    char* write_buf;
    int output_file_bytes = pgm_to_ascii((unsigned char*)output_img, width, height, &write_buf);

    sprintf(control_msg, "%d", output_file_bytes);
    send_msg(sock_fd, control_msg, CONTROL_MSG_LENGTH);
    fprintf(stderr, "Lunghezza file output comunicata al client correttamente\n");

    fprintf(stderr, "Bytes da inviare al client: %d\n", output_file_bytes);

    send_msg(sock_fd, write_buf, output_file_bytes);
    fprintf(stderr, "File inviato al client correttamente\n");
    free(write_buf);

    free((void*)input_img);
    free((void*)output_img);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
       fprintf(stderr, "Sintassi: %s <port_number>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    int ret;

    // ottieni il numero di porta da usare per il server dall'argomento del comando
    uint16_t port_number_no;
    long tmp = strtol(argv[1], NULL, 0); // tenta di convertire una stringa in long
    if (tmp < 1024 || tmp > 49151) {
        fprintf(stderr, "Utilizzare un numero di porta compreso tra 1024 e 49151.\n");
        exit(EXIT_FAILURE);
    }
    port_number_no = htons((uint16_t)tmp);

    // impostazioni per le connessioni in ingresso
    int server_desc = socket(AF_INET , SOCK_STREAM , 0);
    ERROR_HELPER(server_desc, "[MAIN THREAD] Impossibile creare socket server_desc");

    struct sockaddr_in server_addr = {0};
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = port_number_no;

    int reuseaddr_opt = 1; // riavvio rapido del server dopo un crash
    ret = setsockopt(server_desc, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt));
    ERROR_HELPER(ret, "[MAIN THREAD] Impossibile settare l'opzione SO_REUSEADDR per socket server_desc");

    int sockaddr_len = sizeof(struct sockaddr_in);
    ret = bind(server_desc, (struct sockaddr*) &server_addr, sockaddr_len); // binding dell'indirizzo
    ERROR_HELPER(ret, "[MAIN THREAD] Impossibile eseguire bind() su server_desc");

    ret = listen(server_desc, 16); // marca la socket come passiva
    ERROR_HELPER(ret, "[MAIN THREAD] Impossibile eseguire listen() su server_desc");

    // avvio dei working threads
    setup_threads();

    // il server processa le richieste in modo seriale!
    while (1) {
        struct sockaddr_in client_addr = {0};

        // mi preparo ad accettare una connessione in ingresso
        int client_desc = accept(server_desc, (struct sockaddr*)&client_addr, (socklen_t*) &sockaddr_len);
        if (client_desc == -1 && errno == EINTR) continue;
        ERROR_HELPER(client_desc, "[MAIN THREAD] Impossibile eseguire accept() su server_desc");

        // processa la richiesta
        session(client_desc);

        // chiudo connessione in ingresso
        ret = close(client_desc);
        ERROR_HELPER(ret, "[MAIN THREAD] Impossibile chiudere la socket client_desc per la connessione accettata");

    }

    exit(EXIT_SUCCESS);
}
