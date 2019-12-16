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

        // non vogliamo creare più di MAX_CONCURRENCY processi figli in contemporanea
        ret = sem_wait(concurrency_sem);
        ERROR_HELPER(ret, "[MAIN PROCESS] Impossibile fare wait sul semaforo named concurrency_sem");

        pid_t pid = fork();
        if (pid == -1) {
            ERROR_HELPER(-1, "[MAIN PROCESS] Impossibile eseguire fork()");
        } else if (pid == 0) {
            /* Processo figlio: chiudere la listening socket e processare la richiesta */
            pid_t child_pid = syscall(SYS_getpid);

            ret = close(server_desc);
            if (ret == -1) {
                // opzionale: in alternativa all'ERROR_HELPER(), si può gestire l'errore mostrando il PID del child
                fprintf(stderr, "[PROCESS %u] Impossibile chiudere la listening socket socket_desc\n", child_pid);
                exit(EXIT_FAILURE);
            }

            // metodo helper per processare una richiesta in ingresso
            connection_handler(child_pid, client_desc, &client_addr); // chiude client_desc prima di terminare

            // il processo figlio sta per terminare: incremento il semaforo n-ario concurrency_sem
            ret = sem_post(concurrency_sem);
            if (ret == -1) {
                // opzionale: in alternativa all'ERROR_HELPER(), si può gestire l'errore mostrando il PID del child
                fprintf(stderr, "[PROCESS %u] Impossibile fare post sul semaforo named concurrency_sem\n", child_pid);
                exit(EXIT_FAILURE);
            }

            // chiudiamo il semaforo named
            ret = sem_close(concurrency_sem);
            if (ret == -1) {
                // opzionale: in alternativa all'ERROR_HELPER(), si può gestire l'errore mostrando il PID del child
                fprintf(stderr, "[PROCESS %u] Impossibile chiudere il semaforo named concurrency_sem\n", child_pid);
                exit(EXIT_FAILURE);
            }

            // trovandoci in un ciclo while(1) l'istruzione exit è fondamentale!!!
            exit(EXIT_SUCCESS);
        } else {
            /* Processo padre: chiudere socket connessione accettata e prepararsi per nuova accept() */

            ret = close(client_desc);
            ERROR_HELPER(ret, "[MAIN PROCESS] Impossibile chiudere la socket client_desc per la connessione accettata");

            // reinizializzo a zero la struttura dati client_addr
            memset(&client_addr, 0, sizeof(struct sockaddr_in));
        }
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

    /* Semplificazione: dato che il server non prevede un meccanismo di
     * terminazione che rimuova dal sistema i semafori named creati, in
     * ogni esecuzioen facciamo precedere la creazione da una operazione
     * di unlink, di cui NON controlleremo il valore di ritorno (in caso
     * di errore ci aspetteremmo SEM_FAILED con errno == ENOENT). */
    sem_unlink(CONCURRENCY_SEMAPHORE_NAME);

    // creazione semaforo named binario per il logger
    concurrency_sem = sem_open(CONCURRENCY_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0600, MAX_CONCURRENCY);

    if (concurrency_sem == SEM_FAILED) {
        ERROR_HELPER(-1, "[MAIN PROCESS] Impossibile creare il semaforo named concurrency_sem");
    }

    // inizializza meccanismo di logging
    initialize_logger(LOGFILE_NAME);

    // inizia ad accettare connessioni in ingresso sulla porta data
    listen_on_port(port_number_no);

    exit(EXIT_SUCCESS);
}
