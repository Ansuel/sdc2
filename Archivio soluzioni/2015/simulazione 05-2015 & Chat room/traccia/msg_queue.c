#include <stdio.h>
#include <semaphore.h>

#include "common.h"
#include "methods.h"

#define MSG_BUFFER_SIZE 256

// buffer circolare per gestire una coda FIFO (First In First Out)
msg_t* msg_buffer[MSG_BUFFER_SIZE];
int read_index;
int write_index;

/**
 * SUGGERIMENTO: dichiarare qui le variabili globali sem_t necessarie,
 *               e non preoccuparsi della distruzione dei semafori
 **/

/*
 * Inizializza la coda.
 */
void initialize_queue() {

    int ret;

    read_index = 0,
    write_index = 0;

    /**
     * SUGGERIMENTO: inizializzare i semafori necessari per garantire
     *               assenza di race conditions nell'uso della coda
     **/
}

/*
 * Genera un messaggio a partire dagli argomenti e lo inserisce nella coda.
 *
 * Può essere eseguito da più thread contemporaneamente.
 */
void enqueue(const char *nickname, const char *msg) {

    int ret;

    // preparo l'elemento msg_t* msg_data da inserire nella coda
    msg_t* msg_data = (msg_t*)malloc(sizeof(msg_t));
    sprintf(msg_data->nickname, "%s", nickname);
    sprintf(msg_data->msg, "%s", msg);

    /**
     * SUGGERIMENTO: estendere il codice che segue garantendo che le
     *               operazioni di inserimento del puntatore nel buffer
     *               e di update dell'indice di scrittura avvengano in
     *               assenza di race conditions (vedi specifica)
     **/

    msg_buffer[write_index] = msg_data;
    write_index = (write_index + 1) % MSG_BUFFER_SIZE;

}

/*
 * Estrae e restituisce il primo messaggio dalla coda (politica FIFO).
 *
 * Il thread che esegue questo metodo è uno soltanto.
 */

msg_t* dequeue() {

    int ret;
    msg_t *msg = NULL;

    /**
     * SUGGERIMENTO: estendere il codice che segue garantendo che le
     *               operazioni di lettura del puntatore dal buffer e
     *               di update dell'indice di lettura avvengano in
     *               assenza di race conditions (vedi specifica)
     **/

    msg = msg_buffer[read_index];
    read_index = (read_index + 1) % MSG_BUFFER_SIZE;

    return msg;
}
