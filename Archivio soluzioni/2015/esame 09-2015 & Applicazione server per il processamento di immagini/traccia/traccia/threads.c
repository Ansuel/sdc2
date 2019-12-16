#include <pthread.h>
#include <semaphore.h>

#include "common.h"
#include "methods.h"

// variabili globali definite in altri moduli
volatile extern unsigned char* input_img;
volatile extern unsigned char* output_img;
volatile extern int width;
volatile extern int height;

// variabili definite nel modulo corrente
sem_t semaphores[NUM_THREADS];
pthread_t threads[NUM_THREADS];

typedef struct thread_arg_s {
    int index;
} thread_arg_t;

void* working_thread(void* arg) {
    thread_arg_t* thread_arg = (thread_arg_t*) arg;

    int thread_index = thread_arg->index;

    while (1) {
        /** [PUNTO 2]
         *
         * Obiettivo:
         * Controllare l'accesso al corpo del while tramite il semaforo
         * i-esimo associato al thread corrente. Il thread ad ogni
         * iterazione si mette in attesa del main thread tramite il
         * semaforo, esegue le istruzioni presenti nel corpo del while,
         * infine incrementa il semaforo.
         *
         * Note: gestire eventuali errori sui semafori
         *
         **/

        int num_pixels = width*height;
        int chunk_size = num_pixels/NUM_THREADS;
        int start = thread_index * chunk_size;
        int end = (thread_index + 1) * chunk_size; // end-1 è l'ultimo indice valido

        if (thread_index == NUM_THREADS - 1) {
            // l'ultimo thread processa anche i pixel residui della divisione intera
            end += num_pixels % NUM_THREADS;
        }

        int index;
        for (index = start; index < end; ++index) {
            // filtro: costruisci il negativo dell'immagine
            output_img[index] = 255 - input_img[index];
        }

    }

    return NULL;
}

void setup_threads() {

    /** [PUNTO 1A] CREAZIONE DEI SEMAFORI
     *
     * Obiettivo:
     * Creare NUM_THREADS semafori anonimi con valore iniziale 0, usando
     * per il semaforo i-esimo la cella i-esimo dell'array definito nella
     * variabile globale sem_t semaphores[NUM_THREADS]
     *
     * Note:
     * Gestire esplicitamente eventuali errori
     *
     **/


    /** [PUNTO 1B] CREAZIONE DEI WORKING THREADS
     *
     * Obiettivo:
     * Creare NUM_THREADS thread che eseguano il metodo working_thread(),
     * che prende in input un puntatore ad una struttura dati di tipo
     * thread_arg_t contenente l'indice di creazione del thread
     *
     * Note:
     * - gli indici validi per i thread vanno da 0 a NUM_THREADS-1
     * - per la creazione degli oggetti pthread_t si utilizzi l'array
     *   definito nella variabile globale pthread_t threads[NUM_THREADS]
     * - non sono previste operazioni di sincronizzazione sui thread!
     * - gestire esplicitamente eventuali errori
     *
     **/
}
