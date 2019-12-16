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
        int ret = sem_wait(&semaphores[thread_index]);
        ERROR_HELPER(ret, "Errore nella wait sul semaforo");

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

        ret = sem_post(&semaphores[thread_index]);
        ERROR_HELPER(ret, "Errore nella post sul semaforo");
    }

    return NULL;
}

void setup_threads() {

    int i, ret;

    // crea NUM_THREADS semafori binari anonimi nell'array semaphores[]
    for (i = 0; i < NUM_THREADS; ++i) {
        ret = sem_init(&semaphores[i], 0, 0);
        ERROR_HELPER(ret, "Errore nell'inizializzazione del semaforo");
    }

    // crea NUM_THREADS thread passando l'indice come argomento
    for (i = 0; i < NUM_THREADS; ++i) {
        thread_arg_t* thread_arg = malloc(sizeof(thread_arg_t));
        thread_arg->index = i;
        ret = pthread_create(&threads[i], NULL, working_thread, (void*)thread_arg);
        GENERIC_ERROR_HELPER((ret != 0), ret, "Impossible creare un working thread");
        ret = pthread_detach(threads[i]);
        GENERIC_ERROR_HELPER((ret != 0), ret, "Impossible fare detach di un working thread");
    }

}
