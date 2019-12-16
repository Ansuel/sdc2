#include <semaphore.h>

#include "common.h"
#include "methods.h"

#define CHAR_MSG_BUFFER_SIZE 5

char_msg_t* char_msg_buffer[CHAR_MSG_BUFFER_SIZE];

sem_t empty_sem;
sem_t filled_sem;
sem_t write_sem;

int read_index, write_index;

void init_char_msg_buffer() {
    // init sems for prod/cons
    int ret = sem_init(&empty_sem, 0, CHAR_MSG_BUFFER_SIZE);
    ERROR_HELPER(ret, "Errore nell'inizializzazione del semaforo empty_sem");

    ret = sem_init(&filled_sem, 0, 0);
    ERROR_HELPER(ret, "Errore nell'inizializzazione del semaforo filled_sem");

    ret = sem_init(&write_sem, 0, 1);
    ERROR_HELPER(ret, "Errore nell'inizializzazione del semaforo write_sem");

    read_index = 0;
    write_index = 0;
}

char_msg_t *get_char_msg() {
    int ret = sem_wait(&filled_sem);
    ERROR_HELPER(ret, "Errore nella wait sul semaforo filled_sem");

    char_msg_t *char_msg = char_msg_buffer[read_index];
    read_index = (read_index + 1) % CHAR_MSG_BUFFER_SIZE;

    ret = sem_post(&empty_sem);
    ERROR_HELPER(ret, "Errore nella post sul semaforo empty_sem");

    return char_msg;
}

void put_char_msg(char_msg_t *char_msg) {
    int ret = sem_wait(&empty_sem);
    ERROR_HELPER(ret, "Errore nella wait sul semaforo empty_sem");

    ret = sem_wait(&write_sem);
    ERROR_HELPER(ret, "Errore nella wait sul semaforo write_sem");

    char_msg_buffer[write_index] = char_msg;
    write_index = (write_index + 1) % CHAR_MSG_BUFFER_SIZE;

    ret = sem_post(&write_sem);
    ERROR_HELPER(ret, "Errore nella post sul semaforo write_sem");

    ret = sem_post(&filled_sem);
    ERROR_HELPER(ret, "Errore nella post sul semaforo filled_sem");
}

void destroy_char_msg_buffer() {
    int ret = sem_destroy(&empty_sem);
    ERROR_HELPER(ret, "Errore nella distruzione del semaforo empty_sem");

    ret = sem_destroy(&filled_sem);
    ERROR_HELPER(ret, "Errore nella distruzione del semaforo filled_sem");

    ret = sem_destroy(&write_sem);
    ERROR_HELPER(ret, "Errore nella distruzione del semaforo write_sem");
}
