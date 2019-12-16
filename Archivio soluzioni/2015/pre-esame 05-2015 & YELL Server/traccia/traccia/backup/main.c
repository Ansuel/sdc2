#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h"
#include "methods.h"

#define CONCURRENCY_SEMAPHORE_NAME  "/srv_concurrency"
sem_t* concurrency_sem;

void connection_handler(pid_t process_id, int client_desc, struct sockaddr_in *client_addr) {

    // estrai informazioni sul client (in host byte order)
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    uint16_t client_port = ntohs(client_addr->sin_port);

    fprintf(stderr, "[PROCESS %u] Gestisco connessione da %s sulla porta %hu...\n", process_id, client_ip, client_port);

    char msg_buf[MSG_SIZE];

    // invio messaggio di benvenuto al client
    sprintf(msg_buf, "Ciao %s! Sono uno YELL server e stiamo parlando sulla porta %hu.\nTi \"urlerò\" indietro tutto ciò che mi"
                    " manderai.\nSmetterò quando mi invierai %s :-)", client_ip, client_port, BYE_COMMAND);
    size_t msg_len = strlen(msg_buf);

    send_msg(client_desc, msg_buf, msg_len);

    // main loop
    char* bye_command = BYE_COMMAND;
    size_t bye_command_len = strlen(bye_command);
    size_t max_recv_len = MSG_SIZE;

    while (1) {
        /* Ricevo un messaggio dal client */
        msg_len = recv_msg(client_desc, msg_buf, max_recv_len);

        if (msg_len == -1) {
            fprintf(stderr, "[PROCESS %u] Attenzione: il client %s sulla porta %hu ha chiuso la "
                    "connessione in modo inaspettato!\n", process_id, client_ip, client_port);
            exit(EXIT_FAILURE);
        }

        if (msg_len == bye_command_len && !memcmp(msg_buf, bye_command, bye_command_len)) break;

        /* Genera messaggio per file di log */
        log_message_helper(process_id, client_ip, client_port, msg_buf, msg_len);

        /* Invia messaggio di risposta */
        if (msg_len <= MSG_SIZE/2) {
            int i;
            for (i = 0; i < msg_len; ++i) {
                msg_buf[i] = toupper(msg_buf[i]);
                msg_buf[msg_len+i] = '!';
            }
            msg_len = msg_len * 2;
        } else {
            sprintf(msg_buf, "HAI SCRITTO UN MESSAGGIO TROPPO LUNGO!!!");
            msg_len = strlen(msg_buf);
        }

        send_msg(client_desc, msg_buf, msg_len);
    }

    // chiusura connessione
    int ret = close(client_desc);
    if (ret != 0) {
        fprintf(stderr, "[PROCESS %u] Errore: impossibile chiudere la socket per "
                    "%s sulla porta %hu!\n", process_id, client_ip, client_port);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[PROCESS %u] Connessione con %s sulla porta %hu terminata.\n", process_id, client_ip, client_port);
}

void listen_on_port(uint16_t port_number_no) {
    int ret;

    int server_desc;
    struct sockaddr_in server_addr = {0};
    int sockaddr_len = sizeof(struct sockaddr_in); // usato da accept()

    // impostazioni per le connessioni in ingresso
    server_desc = socket(AF_INET , SOCK_STREAM , 0);
    ERROR_HELPER(server_desc, "[MAIN PROCESS] Impossibile creare socket server_desc");

    server_addr.sin_addr.s_addr = INADDR_ANY; // accettiamo connessioni da tutte le interfacce (es. lo 127.0.0.1)
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = port_number_no;

    int reuseaddr_opt = 1; // SO_REUSEADDR permette un riavvio rapido del server dopo un crash
    ret = setsockopt(server_desc, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt));
    ERROR_HELPER(ret, "[MAIN PROCESS] Impossibile settare l'opzione SO_REUSEADDR su socket_desc");

    // binding dell'indirizzo alla socket
    ret = bind(server_desc, (struct sockaddr*) &server_addr, sockaddr_len);
    ERROR_HELPER(ret, "[MAIN PROCESS] Impossibile eseguire bind() su socket_desc");

    // marca la socket come passiva per mettersi in ascolto
    ret = listen(server_desc, MAX_CONN_QUEUE);
    ERROR_HELPER(ret, "[MAIN PROCESS] Impossibile eseguire listen() su socket_desc");

    /* Trattandosi di un server multi-process, allochiamo la variabile in stack:
     * in caso di fork() ogni processo avrà una sua copia distinta! */
    struct sockaddr_in client_addr = {0};

    while (1) {
        // mi preparo ad accettare una connessione in ingresso
        int client_desc = accept(server_desc, (struct sockaddr*)&client_addr, (socklen_t*) &sockaddr_len);
        if (client_desc == -1 && errno == EINTR) continue;
        ERROR_HELPER(client_desc, "[MAIN PROCESS] Impossibile eseguire accept() su socket_desc");

        /** INSERIRE QUI IL CODICE PER CONTROLLARE IL NUMERO DI ACCESSI
         *  CONCORRENTI E PER LANCIARE I PROCESSI FIGLIO CHE SI OCCUPERANNO DI
         *  GESTIRE LE CONNESSIONI CON I CLIENT
         *
         *  Suggerimenti
         *  - dopo aver controllato il numero di accessi concorrenti, bisogna
         *    lanciare un processo figlio che eseguirà la funzione
         *    connection_handler(); dopo che quest'ultima è terminata, la
         *    richiesta del client si può considerare servita
         *  - è possibile ottenere il PID del processo corrente - richiesto
         *    tra gli argomenti per connection_handler() - usando la chiamata
         *    syscall(SYS_getpid)
         *  - sia il processo figlio che il processo padre devono chiudere le
         *    socket che non useranno
         *  - il processo padre deve reinizializzare la struttura dati da usare
         *    per mantenere le informazioni sull'indirizzo del client
         *  - il processo figlio deve terminare una volta chiusa la connessione
         **/

    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
       fprintf(stderr, "Sintassi: %s <port_number>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    // ottieni il numero di porta da usare per il server dall'argomento del comando
    uint16_t port_number_no;
    long tmp = strtol(argv[1], NULL, 0); // tenta di convertire una stringa in long
    if (tmp < 1024 || tmp > 49151) {
        fprintf(stderr, "Utilizzare un numero di porta compreso tra 1024 e 49151.\n");
        exit(EXIT_FAILURE);
    }
    port_number_no = htons((uint16_t)tmp);

    /** INSERIRE QUI IL CODICE PER L'APERTURA DEL SEMAFORO DA USARE PER
     *  CONTROLLARE IL NUMERO DI ACCESSI CONCORRENTI
     *
     * Dato che il server per semplicità non prevede un meccanismo di
     * terminazione che rimuova dal sistema i semafori named creati, in ogni
     * esecuzione facciamo precedere l'apertura da una operazione di rimozione,
     * di cui NON controlleremo il valore di ritorno.
     *
     * Si consiglia di usare la variabile globale concurrency_sem.
     **/

    // inizializza meccanismo di logging
    initialize_logger(LOGFILE_NAME);

    // inizia ad accettare connessioni in ingresso sulla porta data
    listen_on_port(port_number_no);

    exit(EXIT_SUCCESS);
}
