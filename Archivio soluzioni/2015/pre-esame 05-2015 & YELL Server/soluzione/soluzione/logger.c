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

    /* Semplificazione: dato che il server non prevede un meccanismo di
     * terminazione che rimuova dal sistema i semafori named creati, in
     * ogni esecuzioen facciamo precedere la creazione da una operazione
     * di unlink, di cui NON controlleremo il valore di ritorno (in caso
     * di errore ci aspetteremmo SEM_FAILED con errno == ENOENT). */
    sem_unlink(LOG_SEMAPHORE_NAME);

    // creazione semaforo named binario per il logger
    log_sem = sem_open(LOG_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0600, 1);

    if (log_sem == SEM_FAILED) {
        ERROR_HELPER(-1, "Impossibile creare il semaforo named log_sem");
    }

}

void log_message(const char* msg, size_t msg_len) {
    int ret;

    ret = sem_wait(log_sem);
    ERROR_HELPER(ret, "Impossibile fare wait sul semaforo named log_sem");

    // semplificazione: scrittura best-effort su FILE*
    fwrite(msg, 1, msg_len, log_file);
    fflush(log_file);

    ret = sem_post(log_sem);
    ERROR_HELPER(ret, "Impossibile fare post sul semaforo named log_sem");
}
