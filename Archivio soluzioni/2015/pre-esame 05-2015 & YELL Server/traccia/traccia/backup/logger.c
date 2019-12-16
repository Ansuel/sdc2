#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "methods.h"

FILE*   log_file;

#define LOG_SEMAPHORE_NAME  "/srv_logger"
sem_t*  log_sem;

void initialize_logger(const char* logfile_name) {
    // se il file di log esiste già, il suo contenuto viene troncato
    log_file = fopen(logfile_name, "w");
    if (log_file == NULL) ERROR_HELPER(-1, "Impossibile creare file di log");

     /** INSERIRE QUI IL CODICE PER L'APERTURA DEL SEMAFORO DA USARE PER
     *  CONTROLLARE L'ACCESSO AL FILE DI LOG
     *
     * Dato che il server per semplicità non prevede un meccanismo di
     * terminazione che rimuova dal sistema i semafori named creati, in ogni
     * esecuzione facciamo precedere l'apertura da una operazione di rimozione,
     * di cui NON controlleremo il valore di ritorno.
     *
     * Si consiglia di usare la variabile globale log_sem.
     **/

}

void log_message(const char* msg, size_t msg_len) {
    int ret;

    /** INSERIRE QUI IL CODICE PER GESTIRE L'ACCESSO AL FILE DI LOG
     *
     * La scrittura su file di log è una sezione critica da proteggere tramite
     * un opportuno uso del semaforo log_sem.
     **/

    // semplificazione: scrittura best-effort su FILE*
    fwrite(msg, 1, msg_len, log_file);
    fflush(log_file);

}
