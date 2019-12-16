#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include "common.h"

int pipefd[2];

int write_to_pipe(int fd, const void *data, size_t data_len) {
    
    /**
     * SOLUZIONE
     * 
     * Obiettivi:
     * - scrivere sul descrittore i primi 'data_len' bytes contenuti in data
     * - gestire eventuali interruzioni ed errori
     * - assicurarsi che tutti i 'data_len' byte siano stati scritti
     * - restituire il numero di bytes scritti
     **/
     
    int ret;
    int written_bytes = 0;
    while (written_bytes < data_len) {
        ret = write(fd, data + written_bytes, data_len - written_bytes);
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Errore nella scrittura su pipe");
        written_bytes += ret;
    }
    return written_bytes;
}

int read_from_pipe(int fd, void *data, size_t data_len) {
    
    /**
     * SOLUZIONE
     * 
     * Obiettivi:
     * - leggere dal descrittore 'data_len' bytes e memorizzarli in data
     * - gestire eventuali interruzioni ed errori
     * - in caso di chiusura inaspettata della pipe, ritornare immediatamente -1
     * - assicurarsi che tutti i 'data_len' bytes siano stati letti
     * - restituire il numero di bytes letti
     **/
     
    int ret;
    int read_bytes = 0;
    while (read_bytes < data_len) {
        ret = read(fd, data + read_bytes, data_len - read_bytes);
        if (ret == -1 && errno == EINTR) continue;
        ERROR_HELPER(ret, "Errore nella lettura da pipe");
        if (ret == 0) return -1;
        read_bytes += ret;
    }
    return read_bytes;
}

/**
 * Questa funzione scrive nell'array di interi 'data' un numero pari a 
 * 'elem_count' di interi, tutti aventi lo stesso valore 'value'.
 **/
void create_msg(int *data, int elem_count, int value) {
    int i;
    for (i = 0; i < elem_count; i++)
        data[i] = value;
}

/**
 * Questa funzione verifica se un messaggio sia integro controllando se tutti 
 * gli 'elem_count' valori interi contenuti nell'array 'data' abbiano lo 
 * stesso valore.
 * In caso positivo, ritorna 1 (true), altrimenti 0 (false).
 **/
int is_msg_ok(const int *data, int elem_count) {
    int i;
    for (i = 0; i < elem_count; i++)
        if (data[0] != data[i])
            return 0;
    return 1;
}

void child(int child_id) {
    
    int data[MSG_SIZE];
    
    /**
     * SOLUZIONE
     * 
     * Obiettivi:
     * - chiudere i descrittori non necessari
     * - creare un named semaphore con nome 'SEMAPHORE_NAME' per gestire la 
     *   mutua esclusione nella scrittura su pipe
     * - nel caso il named semaphore già esista, va semplicemente aperto
     * - inviare MSG_COUNT messaggi usando la funzione write_to_pipe(); ogni 
     *   messaggio va prima creato usando la funzione create_msg(), specificando
     *   'child_id' come valore dell'argomento 'elem_count'; per memorizzare i 
     *   messaggi, si consiglia di usare l'array 'data' già dichiarato; per 
     *   gestire la mutua esclusione nella scrittura su pipe, usare il named 
     *   semaphore creato prima
     * - NB: il numero di byte da scrivere è diverso dal numero di interi 
     *   contenuti nel messaggio!!!
     * - infine, chiudere i descrittori rimanenti ed il named semaphore
     * - gestire eventuali errori
     **/
    
    int ret = close(pipefd[0]);
    ERROR_HELPER(ret, "Errore nella chiusura del descrittore di lettura della pipe");
    
    sem_t* named_semaphore = sem_open(SEMAPHORE_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (named_semaphore == SEM_FAILED && errno == EEXIST)        
        named_semaphore = sem_open(SEMAPHORE_NAME, 0);
    if (named_semaphore == SEM_FAILED) ERROR_HELPER(-1, "Impossibile creare il named semaphore");
    
    int i;
    for (i = 0; i < MSG_COUNT; i++) {
        create_msg(data, MSG_SIZE, child_id);
        
        ret = sem_wait(named_semaphore);
        ERROR_HELPER(ret, "Errore nella wait");
        
        write_to_pipe(pipefd[1], data, MSG_SIZE * sizeof(int));
        
        ret = sem_post(named_semaphore);
        ERROR_HELPER(ret, "Errore nella post");
        
        printf("Process %d sent msg #%d\n", child_id, i);
    }
    
    ret = close(pipefd[1]);
    ERROR_HELPER(ret, "Errore nella chiusura del descrittore di scrittura della pipe");
    
    ret = sem_close(named_semaphore);
    ERROR_HELPER(ret, "Errore nella chiusura del named semaphore");
}

void parent() {
    
    int data[MSG_SIZE];
    
    /**
     * SOLUZIONE
     * 
     * Obiettivi:
     * - chiudere i descrittori non necessari
     * - ricevere CHILDREN_COUNT*MSG_COUNT messaggi usando la funzione 
     *   read_from_pipe(); per memorizzare i messaggi, si consiglia di usare 
     *   l'array 'data' già dichiarato; verificare se ogni messaggio sia 
     *   integro usando la funzione is_msg_ok(), ed in caso negativo stampare 
     *   un messaggio su terminale
     * - NB: il numero di byte da leggere è diverso dal numero di interi 
     *   contenuti nel messaggio!!!
     * - infine, chiudere i descrittori rimanenti, attendere il termine di tutti
     *   i processi figlio, e rimuovere il named semaphore
     * - gestire eventuali errori
     **/
     
    int ret = close(pipefd[1]);
    ERROR_HELPER(ret, "Errore nella chiusura del descrittore di scrittura della pipe");
    
    int i;
    for (i = 0; i < CHILDREN_COUNT * MSG_COUNT; i++) {
        ret = read_from_pipe(pipefd[0], data, MSG_SIZE * sizeof(int));
        ERROR_HELPER(ret, "Errore nella lettura da pipe");
        
        printf("Main process received msg #%d from process %d\n", i, data[0]);
        if (!is_msg_ok(data, MSG_SIZE))
            printf("WARN: msg #%d is malformed!\n", i);
    }
    
    ret = close(pipefd[0]);
    ERROR_HELPER(ret, "Errore nella chiusura del descrittore di lettura della pipe");
    
    for (i = 0; i < CHILDREN_COUNT; i++) {
        ret = wait(NULL);
        ERROR_HELPER(ret, "Errore nella wait");
    }
    
    ret = sem_unlink(SEMAPHORE_NAME);
    ERROR_HELPER(ret, "Errore nella rimozione del named semaphore");
}

int main(int argc, char* argv[]) {
    
    /**
     * SOLUZIONE
     * 
     * Obiettivi:
     * - assicurarsi che non esista un named semaphore di nome SEMAPHORE_NAME
     * - creare una pipe e salvare i descrittori nell'array 'pipefd' già 
     *   dichiarato
     * - creare CHILDREN_COUNT processi figlio paralleli; il processo figlio 
     *   i-esimo deve invocare la funzione child(i)
     * - dopo aver lanciato i processi figlio, il processo padre deve eseguire 
     *   la funzione parent()
     * - NB: nessun processo figlio deve creare altri processi o eseguire la 
     *   funzione parent()
     * - gestire eventuali errori
     **/
    
    sem_unlink(SEMAPHORE_NAME);
    
    int ret = pipe(pipefd);
    ERROR_HELPER(ret, "Impossibile creare la pipe");
    
    int i;
    pid_t pid;
    for (i = 0; i < CHILDREN_COUNT; i++) {
        pid = fork();
        if (pid == -1) ERROR_HELPER(-1, "Errore nella creazione di un figlio");
        if (pid == 0) {
            child(i);
            break;
        }
    }
    
    if (i == CHILDREN_COUNT)
        parent();
    
    return EXIT_SUCCESS;
}
